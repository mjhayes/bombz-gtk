/*
 * This file is part of bombz-gtk.
 *
 * bombz-gtk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bombz-gtk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bombz-gtk.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtkmm/main.h>
#include "bombz.h"

// Program entry point!  Yay!
int main(int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);	// Instantiates gtk, passes default arguments
	
	bombz bombwin(9, 9);		// Instantiate our main window
	Gtk::Main::run(bombwin);	// Run the main window
	
	return 0;
}
