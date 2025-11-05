// Ayayay el regional
////////////////////////////////////////////////////////////
//       | |                          //           _____  //
//       | | _____  _____ ___  __ _   //    _   __/ ___/  //
//   _   | |/ _ \ \/ / __/ __|/ _` |  //   | | / / __ \   //
//  | |__| |  __/>  <\__ \__ \ (_| |  //   | |/ / /_/ /   //
//   \____/ \___/_/\_\___/___/\__,_|  //   |___/\____/    //
////////////////////////////////////////////////////////////
//                     Jexssa 4205J                       //
//                      Polirobots                        //
//                    Arquitectura #6                     //
//                        gmaxd                           //
////////////////////////////////////////////////////////////
#include "main.h"
#include "screen.h"
#include "filereader.h"
using namespace pros;

void initialize(){
	show_logo("/usd/logo.ipros");
	delay(5000);
	stop_drawing();
	screen::erase();
	lcd::initialize();
}