from PIL import Image
import struct
import hashlib

MAGIC_START = b"imgPROS"
MAGIC_END = b"End imgPROS"

def png_to_imgpros(input_path, output_path, version="1.00.000", verify=True):
    img = Image.open(input_path).convert("RGB")
    width, height = img.size
    pixels = img.load()

    with open(output_path, "wb") as f:
        # Escribir cabecera
        f.write(MAGIC_START)
        f.write(struct.pack("<HH", width, height))

        block_count = 0
        total_pixels = 0

        # Procesar por bloques 16x16
        for y in range(0, height, 16):
            for x in range(0, width, 16):
                # Tamaño real del bloque (puede ser <16 si está al borde)
                block_w = min(16, width - x)
                block_h = min(16, height - y)

                # Recolectar colores del bloque
                colors = [
                    pixels[xx, yy]
                    for yy in range(y, y + block_h)
                    for xx in range(x, x + block_w)
                ]

                first_color = colors[0]
                is_uniform = all(c == first_color for c in colors)

                # Escribir delimitador de inicio de bloque (opcional pero útil)
                f.write(b"\xAA")  # marcador de bloque (0xAA = 170)

                if is_uniform:
                    flag = 0
                    f.write(struct.pack("<B", flag))
                    x1, y1 = x, y
                    x2, y2 = x + block_w - 1, y + block_h - 1
                    r, g, b = first_color
                    f.write(struct.pack("<HHHHBBB", x1, y1, x2, y2, r, g, b))
                    written = 1 + 2*4 + 3 + 1  # flag + coords + RGB + marker
                else:
                    flag = 1
                    f.write(struct.pack("<B", flag))
                    for yy in range(y, y + block_h):
                        for xx in range(x, x + block_w):
                            r, g, b = pixels[xx, yy]
                            f.write(struct.pack("<HHBBB", xx, yy, r, g, b))
                    written = 1 + (block_w * block_h * 7) + 1

                # Final de bloque (marcador)
                f.write(b"\x55")  # 0x55 = 85 (complemento de 0xAA)
                block_count += 1
                total_pixels += block_w * block_h

               # Footer
        f.write(MAGIC_END)
        f.write(version.encode("utf-8"))
        f.close()  # ✅ Cerramos antes de leer

    # Calcular y escribir checksum
    with open(output_path, "rb") as fr:
        file_data = fr.read()
        checksum = hashlib.sha1(file_data).digest()

    with open(output_path, "ab") as fw:
        fw.write(checksum)

    print(f"[OK] Archivo generado: {output_path} ({width}x{height})")
    print(f"Bloques: {block_count} | Total píxeles: {total_pixels}")
    print(f"Checksum SHA1: {checksum.hex()}")

    if verify:
        verify_imgpros(output_path)


# ---------------------------------------------------------------
# 🔍 Verificador integrado (detector de corrupción estructural)
# ---------------------------------------------------------------

def verify_imgpros(path):
    try:
        with open(path, "rb") as f:
            data = f.read()

        if not data.startswith(MAGIC_START):
            raise ValueError("Cabecera inválida: no inicia con 'imgPROS'.")

        # Validar checksum
        content = data[:-20]  # sin checksum
        stored_checksum = data[-20:]
        calc_checksum = hashlib.sha1(content).digest()
        if stored_checksum != calc_checksum:
            raise ValueError("Checksum no coincide (archivo corrupto).")

        # Buscar final
        end_index = content.find(MAGIC_END)
        if end_index == -1:
            raise ValueError("No se encontró 'End imgPROS' en el archivo.")

        print("[VERIFICACIÓN OK] Archivo válido y sin corrupción estructural.")

    except Exception as e:
        print(f"[ERROR] Archivo corrupto o malformado: {e}")

# ---------------------------------------------------------------
# Ejemplo de uso
# ---------------------------------------------------------------
if __name__ == "__main__":
    png_to_imgpros(
        "3d_party/balatro.png",
        "3d_party/balatro.ipros",
        version="1.03.000",
        verify=True
    )
