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

#include "scoreboard.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// Used for sorting the scoreboard.
bool scoreboard::compare_score(Glib::ustring str1, Glib::ustring str2)
{
	int c1, c2;
	long t1, t2;
	double btratio1, btratio2;
	
	// Parse score 1
	Glib::ustring score1 = str1.substr(str1.rfind(":") + 1, str1.length());
	Glib::ustring score1c = score1.substr(0, score1.find("/"));
	Glib::ustring score1t = score1.substr(score1.find("/") + 1, score1.length());
	
	// Parse score 2
	Glib::ustring score2 = str1.substr(str1.rfind(":") + 1, str1.length());
	Glib::ustring score2c = score2.substr(0, score2.find("/"));
	Glib::ustring score2t = score2.substr(score2.find("/") + 1, score2.length());
	
	// Convert strings to integers.
	std::istringstream	ss1c(score1c.raw()),
						ss1t(score1t.raw()),
						ss2c(score2c.raw()),
						ss2t(score2t.raw());
	ss1c >> c1;
	ss1t >> t1;
	ss2c >> c2;
	ss2t >> t2;
	
	btratio1 = (double) t1 / (double) c1;
	btratio2 = (double) t2 / (double) c2;
	
	if (btratio1 < btratio2) return true;
	else return false;
}

// Ask the high scorer for their name.
Glib::ustring scoreboard::get_high_scorer(bombz *b, long t, int c)
{
	Gtk::Dialog hiscore("High Score", *b, true, true);
	Gtk::VBox *vbmain = hiscore.get_vbox();
	Gtk::HBox namebox;
	Gtk::Label lname("Enter name: ");
	Gtk::Entry ename;
	Glib::ustring level;
	
	// Set entry properties.
	ename.set_max_length(10);
	ename.set_width_chars(10);
	ename.set_activates_default(true);
	
	// Pack the shiz.
	namebox.set_border_width(6);
	namebox.pack_start(lname);
	namebox.pack_start(ename);
	vbmain->pack_start(namebox);
	
	// Set the dialog's padding
	hiscore.set_border_width(10);
	
	// Add buttons
	hiscore.add_button(Gtk::Stock::OK, 101);
	hiscore.set_default_response(101);
	
	// Show the dialog.
	hiscore.show_all_children();
	hiscore.run();
	
	// Check if nothing was entered for name.
	if (ename.get_text().compare("") == 0)
		ename.set_text("None");
	
	// Set skill letter
	if (b->get_skill() == 0)
		level = "e";
	else if (b->get_skill() == 1)
		level = "m";
	else if (b->get_skill() == 2)
		level = "h";
	else if (b->get_skill() == 3)
		level = "c";
	
	return Glib::ustring::compose("%1:%2:%3/%4", level, ename.get_text(), c, t);
}

// Check if we have a high score.
void scoreboard::check_high_scores(bombz *b, long t, int c)
{
	// Oh.  My.  God.  Hell must have frozen over if by random chance,
	// no bombs were put on the board.  Show the dialog.
	if (c == 0)
	{
		// We won't show this for custom games,
		// but we still don't want to compute the score.
		if (b->get_skill() == 3)
			return;
			
		Gtk::MessageDialog hell(*b, "Hell must have frozen over.");
		hell.set_secondary_text("This is a rare occasion.\n"
								"The nearly statistically impossible occurred:\n"
								"You got a game with zero bombs.\n"
								"Please go to http://code.google.com/p/bombz-gtk/\n"
								"and send us a screenshot of this game!");
		hell.run();
		
		// Stop so we don't try to divide by zero.
		return;
	}
	
	std::list<Glib::ustring> rec[4];
	std::string file, str, level, name, score, bombcount, ttime;
	bool flag = false;
	int bc;
	long time;
	double btratio, btratio_cur;
	Glib::ustring flagstr;
	
	// Set score file path.
	file = getenv("HOME");
	file.append("/.bombz-gtk");
	
	// Start the file input stream.
	std::ifstream in(file.c_str());
	
	// Make sure we could open the file.
	if (in.is_open())
	{
		// Read through until we hit the end.
		while (!in.eof())
		{
			std::getline(in, str);
			
			// Parse out needed parameters.
			level = str.substr(0, str.find(":"));
			name = str.substr(str.find(":") + 1, str.rfind(":") - 2);
			score = str.substr(str.rfind(":") + 1, str.length());
			bombcount = score.substr(0, score.find("/"));
			ttime = score.substr(score.find("/") + 1, str.length());
			
			// Convert these strings to integers.
			std::istringstream	ss1(bombcount), ss2(ttime);
			ss1 >> bc;
			ss2 >> time;
			
			// Bombs:Time ratio.
			btratio = (double) time / (double) bc;
			btratio_cur = (double) t / (double) c;
			
			// Easy
			if (level.compare("e") == 0)
			{
				if (b->get_skill() == 0 && !flag && btratio_cur < btratio)
				{
					flagstr = get_high_scorer(b, t, c);
					rec[0].push_back(flagstr);
					flag = true;
				}
				rec[0].push_back(str);
			}
			// Medium
			else if (level.compare("m") == 0)
			{
				if (b->get_skill() == 1 && !flag && btratio_cur < btratio)
				{
					flagstr = get_high_scorer(b, t, c);
					rec[1].push_back(flagstr);
					flag = true;
				}
				rec[1].push_back(str);
			}
			// Hard
			else if (level.compare("h") == 0)
			{
				if (b->get_skill() == 2 && !flag && btratio_cur < btratio)
				{
					flagstr = get_high_scorer(b, t, c);
					rec[2].push_back(flagstr);
					flag = true;
				}
				rec[2].push_back(str);
			}
			// Custom
			else if (level.compare("c") == 0)
			{
				if (b->get_skill() == 3 && !flag && btratio_cur < btratio)
				{
					flagstr = get_high_scorer(b, t, c);
					rec[3].push_back(flagstr);
					flag = true;
				}
				rec[3].push_back(str);
			}
		}
		
		in.close();
	}
	// File probably didn't exist.
	else
	{
		flagstr = get_high_scorer(b, t, c);
		rec[b->get_skill()].push_back(flagstr);
		flag = true;
	}
	
	// High score because we don't have 5 yet.
	if (!flag && rec[b->get_skill()].size() < 5)
	{
		flagstr = get_high_scorer(b, t, c);
		rec[b->get_skill()].push_back(flagstr);
		flag = true;
	}
	
	// Open file write stream.
	std::ofstream out(file.c_str());
	
	for (int i = 0; i < 4; i++)
	{
		// We don't want to deal with an empty list.
		if (rec[i].empty())
			continue;
		
		// If we have more than 5 scores, pop off the lowest score.
		if (rec[i].size() > 5)
		{
			rec[i].sort(compare_score);
			rec[i].pop_back();
		}
		
		// Write lines to file.
		while (!rec[i].empty())
		{
			out << rec[i].front() << "\n";
			rec[i].pop_front();
		}
	}
	
	out.close();
	
	// We got a new high score, so display the
	// scoreboard.
	if (flag)
		show_scoreboard(b, b->get_skill(), flagstr.raw());
}

// Scoreboard dialog
void scoreboard::show_scoreboard(bombz *b, int page, std::string sstr)
{
	Gtk::Dialog scoreboard("Scoreboard", *b, false, true);
	Gtk::VBox *vbmain = scoreboard.get_vbox(), scorebox[4], labelbox;
	Gtk::Notebook notebook;
	Gtk::Label labels[20], bigtitle;
	std::list<Glib::ustring> names[4];
	std::string str, level, name, score, bombcount, ttime;
	Glib::ustring timestr, labelstr;
	int i, j, k, left, bc, min, pos = 0;
	long time;
	double btratio, sec;
	bool has_bolded = false;
	
	// Set score file path.
	str = getenv("HOME");
	str.append("/.bombz-gtk");
	
	// Start the file input stream.
	std::ifstream in(str.c_str());
	
	// Make sure we could open the file.
	if (in.is_open())
	{
		// Read through until we hit the end.
		while (!in.eof())
		{
			std::getline(in, str);
			level = str.substr(0, str.find(":"));
			
			// Easy
			if (level.compare("e") == 0)
				names[0].push_back(str);
			// Medium
			else if (level.compare("m") == 0)
				names[1].push_back(str);
			// Hard
			else if (level.compare("h") == 0)
				names[2].push_back(str);
			// Custom
			else if (level.compare("c") == 0)
				names[3].push_back(str);
		}
		
		in.close();
	}
	
	// Sort each names list.
	for (i = 0; i < 4; i++)
		if (names[i].size() > 1)
			names[i].sort(scoreboard::compare_score);
	
	// The following creates the labels we need
	for (i = 0; i < 4; i++)
	{
		left = 0;
		
		// Set labels for names of current difficulty.
		while (!names[i].empty() && left < 5)
		{
			str = names[i].front();
			name = str.substr(str.find(":") + 1, str.rfind(":") - 2);
			score = str.substr(str.rfind(":") + 1, str.length());
			bombcount = score.substr(0, score.find("/"));
			ttime = score.substr(score.find("/") + 1, str.length());
			
			// Convert these strings to integers.
			std::istringstream	ss1(bombcount), ss2(ttime);
			ss1 >> bc;
			ss2 >> time;
			
			// Bombs:Time ratio.
			btratio = ((double) time / 1000.0) / (double) bc;
			
			min = int((double) time / 1000.0 / 60.0);
			sec = ((double) time / 1000.0) - (double) min * 60.0;
	
			// We only wanna show minutes if we have some.  Right?  ;[
			if (min > 0)
				timestr = Glib::ustring::compose("%1 min %2 sec", min, sec);
			// Only print seconds
			else
				timestr = Glib::ustring::compose("%1 sec", sec);
			
			// Assemble the label string.
			labelstr = Glib::ustring::compose("%1 - %2 (%3 bombs in %4)", name, btratio, bc, timestr);
			
			// If this is our newest high score, bold it.
			if (sstr.compare(str) == 0 && !has_bolded)
			{
				labels[pos].set_markup("<span weight='bold'>" + labelstr + "</span>");
				has_bolded = true;
			}
			// Show score normal.
			else
				labels[pos].set_label(labelstr);
				
			names[i].pop_front();
			scorebox[i].pack_start(labels[pos]);
			
			left++; pos++;
		}
		
		// Fill remaining name slots with dummy entries.
		for (k = left; k < 5; k++)
		{
			labels[pos].set_label("None - None");
			scorebox[i].pack_start(labels[pos]);
			pos++;
		}
	}
	
	// Set padding on each scorebox
	scorebox[0].set_border_width(10);
	scorebox[1].set_border_width(10);
	scorebox[2].set_border_width(10);
	scorebox[3].set_border_width(10);
	
	// Add pages to the notebook.
	notebook.append_page(scorebox[0], "Easy");
	notebook.append_page(scorebox[1], "Medium");
	notebook.append_page(scorebox[2], "Hard");
	notebook.append_page(scorebox[3], "Custom");
	
	// Big label
	bigtitle.set_markup("<span size='xx-large' weight='bold'>High Scores</span>");

	// Set the dialog's padding
	scoreboard.set_border_width(10);
	
	// Pack on the frames
	labelbox.pack_start(bigtitle);
	labelbox.pack_start(notebook);
	labelbox.set_border_width(6);
	labelbox.set_spacing(10);
	vbmain->pack_start(labelbox);
	
	// Add buttons
	scoreboard.add_button(Gtk::Stock::OK, 101);
	scoreboard.set_default_response(101);
	
	// Make all children visible.
	scoreboard.show_all_children();
	
	// Set current notebook page (has to be after widget is shown)
	notebook.set_current_page(page);
	
	// Run the dialog.
	scoreboard.run();
}
