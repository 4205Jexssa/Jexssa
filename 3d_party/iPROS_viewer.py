from PIL import Image, ImageDraw
import sys
import struct
import os
import hashlib

def read_bytes(f, n):
    """Lee exactamente n bytes o lanza ValueError con la posición correcta."""
    data = f.read(n)
    if len(data) != n:
        pos = f.tell()
        raise ValueError(f"Error: fin inesperado de archivo en byte {pos - len(data)} (0x{pos - len(data):X})")
    return data

def _read_flag_skip_markers(f):
    """
    Lee el siguiente byte útil que representa un flag.
    Ignora marcadores opcionales 0xAA (inicio bloque), 0x55 (fin bloque), 0xBE (padding).
    Devuelve el valor (entero 0..255) y la posición del byte leído (antes de avanzar).
    Si encuentra EOF devuelve None.
    """
    while True:
        pos = f.tell()
        b = f.read(1)
        if not b:
            return None, pos
        val = b[0]
        if val in (0xAA, 0x55, 0xBE):
            # marcador opcional: simplemente saltarlo
            continue
        return val, pos

def read_ipros(path, verbose=True):
    """
    Lee un archivo .ipros (compatible con el generador robusto).
    Devuelve PIL.Image si OK, o lanza ValueError con descripción y offset.
    """
    with open(path, "rb") as f:
        # --- header magic ---
        magic = read_bytes(f, 7)
        if magic != b"imgPROS":
            raise ValueError(f"Magic header inválido en byte 0 (esperado b'imgPROS', encontrado {magic})")

        # --- dims ---
        width, height = struct.unpack("<HH", read_bytes(f, 4))
        if verbose:
            print(f"Dimensiones: {width}x{height}")

        if width == 0 or height == 0 or width > 8192 or height > 8192:
            raise ValueError(f"Dimensiones inválidas ({width}x{height}) en byte {f.tell()-4} (0x{f.tell()-4:X})")

        # Prepara imagen
        img = Image.new("RGB", (width, height))
        draw = ImageDraw.Draw(img)

        # Recorre exactamente los bloques en orden raster (misma lógica que el generador)
        blocks_x = (width + 15) // 16
        blocks_y = (height + 15) // 16
        block_idx = 0

        for by in range(blocks_y):
            for bx in range(blocks_x):
                block_idx += 1
                x = bx * 16
                y = by * 16
                block_w = min(16, width - x)
                block_h = min(16, height - y)

                # Leer next flag (ignorando marcadores)
                flag, flag_pos = _read_flag_skip_markers(f)
                if flag is None:
                    raise ValueError(f"EOF inesperado al leer flag del bloque #{block_idx} (esperaba flag en byte {flag_pos})")

                # Si encontramos 'E' (posible inicio de "End imgPROS") ahí algo está mal:
                # eso indicaría que faltan bloques (archivo truncado o desincronizado)
                if flag == ord('E'):
                    raise ValueError(f"Se encontró 'E' (posible 'End imgPROS') en lugar de flag para bloque #{block_idx} en byte {flag_pos} (archivo truncado o estructura inconsistente).")

                if flag == 0:
                    # Bloque uniforme: leer 11 bytes: x1,y1,x2,y2 (4*2) + r,g,b (3)
                    try:
                        # Usamos read_bytes para asegurar que hay suficientes bytes
                        raw = read_bytes(f, 11)
                        x1, y1, x2, y2, r, g, b = struct.unpack("<HHHHBBB", raw)
                    except Exception as e:
                        raise ValueError(f"Error leyendo bloque uniforme #{block_idx} en byte {f.tell()} (0x{f.tell():X}): {e}")

                    # Validación simple de coordenadas (no fatal, pero lo avisamos)
                    if not (x1 <= x2 and y1 <= y2):
                        raise ValueError(f"Coordenadas inválidas en bloque #{block_idx} en byte {flag_pos}: ({x1},{y1})-({x2},{y2})")

                    # Pintar
                    for yy in range(y1, y2 + 1):
                        for xx in range(x1, x2 + 1):
                            if 0 <= xx < width and 0 <= yy < height:
                                draw.point((xx, yy), (r, g, b))

                elif flag == 1:
                    # Bloque de píxeles: el generador escribió exactamente block_w * block_h píxeles,
                    # por lo que aquí debemos leer esa cantidad (no asumir 256).
                    expected_pixels = block_w * block_h
                    bytes_needed = expected_pixels * 7  # cada pixel = 2+2+1+1+1 = 7 bytes
                    # Leemos en un bucle por si el archivo está truncado
                    read = 0
                    try:
                        while read < bytes_needed:
                            remaining = bytes_needed - read
                            chunk = f.read(min(remaining, 4096))
                            if not chunk:
                                raise ValueError(f"EOF inesperado leyendo píxeles del bloque #{block_idx}. Leídos {read} de {bytes_needed} bytes. Pos {f.tell()}")
                            read += len(chunk)
                            # Para procesar necesitamos agrupar exactamente en trozos de 7 bytes.
                            # Mejor strategy: acumular en buffer y desempaquetar.
                            # Pero para simplicidad y robustez, re-seek backwards y usar read_bytes para consumir pixel a pixel:
                            # retrocedemos lo leído y procesamos pixel a pixel con read_bytes
                            f.seek(-read, os.SEEK_CUR)
                            pixels_to_read = remaining + (read - remaining) if False else (read)  # will be overridden
                            # Simplificar: procesar pixel a pixel:
                            for p in range(expected_pixels):
                                # For each pixel try to read 7 bytes; if cannot, exception from read_bytes triggers
                                px_raw = f.read(7)
                                if len(px_raw) < 7:
                                    raise ValueError(f"EOF inesperado en píxel {p} del bloque #{block_idx} (pos {f.tell()-len(px_raw)})")
                                px = struct.unpack("<HHBBB", px_raw)
                                px_x, px_y, r, g, b = px
                                if 0 <= px_x < width and 0 <= px_y < height:
                                    draw.point((px_x, px_y), (r, g, b))
                            # después de procesar pixel a pixel salimos del ciclo de lectura
                            break
                    except ValueError as e:
                        raise ValueError(f"Error leyendo bloque de píxeles #{block_idx} en byte {f.tell()} (0x{f.tell():X}): {e}")

                else:
                    # Flag inválido (no 0 ni 1). Este caso ocurre si el archivo no fue generado por el convertidor
                    # o si hay corrupción. Reportamos ubicación exacta.
                    raise ValueError(f"Flag desconocido ({flag}) en byte {flag_pos} (0x{flag_pos:X}) para bloque #{block_idx}")

        # Al terminar TODOS los bloques según ancho/alto, ahora debemos encontrar 'End imgPROS'
        # Saltar posibles marcadores antes del tail
        # Leer exactamente 10 bytes esperando "End imgPROS"
        # Pero primero consumimos bytes de padding/markers si hay
        # Avanzamos hasta encontrar 'E' que inicie la cadena "End imgPROS"
        while True:
            pos = f.tell()
            b = f.read(1)
            if not b:
                raise ValueError(f"EOF antes de 'End imgPROS' en byte {pos} (0x{pos:X})")
            if b[0] in (0xAA, 0x55, 0xBE):
                continue
            if b[0] == ord('E'):
                # volvemos un byte y leemos 10 bytes completos
                f.seek(-1, os.SEEK_CUR)
                tail = read_bytes(f, 11)
                if tail != b"End imgPROS":
                    raise ValueError(f"Cabecera final corrupta en byte {f.tell()-10} (0x{f.tell()-10:X}), encontrado {tail}")
                # Si tail coincide, lo siguiente es versión + posible checksum
                rest = f.read()  # todo lo que quede
                # Si hay >=20 bytes, interpretamos los últimos 20 como checksum SHA1
                version_text = ""
                checksum_stored = None
                if len(rest) >= 20:
                    checksum_stored = rest[-20:]
                    try:
                        version_text = rest[:-20].decode("utf-8", errors="ignore").strip()
                    except Exception:
                        version_text = "(sin versión legible)"
                else:
                    version_text = rest.decode("utf-8", errors="ignore").strip()
                if verbose and version_text:
                    print(f"Versión declarada: {version_text}")
                # Validar checksum si existe
                if checksum_stored:
                    with open(path, "rb") as cf:
                        data_full = cf.read()
                    data_content = data_full[:-20]
                    checksum_calc = hashlib.sha1(data_content).digest()
                    if checksum_calc == checksum_stored:
                        if verbose:
                            print(f"[OK] Checksum SHA1 válido: {checksum_calc.hex()}")
                    else:
                        raise ValueError(f"Checksum no coincide: almacenado {checksum_stored.hex()} vs calculado {checksum_calc.hex()}")
                return img
            else:
                # hay bytes que no son marcadores ni 'E' — si aparecen aquí, probablemente el archivo tiene basura después
                # pero no queremos bucle infinito: volvemos una posición y continuamos (dejar que el next read avance).
                # sin embargo, esto normalmente no debería pasar: marcadores deberían ser usados.
                # Seguimos buscando.
                f.seek(-0, os.SEEK_CUR)
                continue


def visualize_ipros(path):
    try:
        img = read_ipros(path)
        print(f"[OK] Archivo válido: {path}")
        img.show()  # Muestra la imagen reconstruida
        # img.save(path.replace(".ipros", "_decoded.png"))  # Si quieres guardarla
    except Exception as err:
        print(f"[ERROR] {err}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python iPROS_viewer.py <archivo.ipros>")
        sys.exit(1)

    visualize_ipros(sys.argv[1])
