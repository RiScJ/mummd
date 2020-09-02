#include "article.hpp"

#include <fstream>
#include <algorithm>
#include <functional>
#include <iomanip>

/* ************************************************************************* */

Article::Article(std::string fp) {
	std::ifstream fi;
	fi.open(_src_dir + fp, std::ios::in);
	if (fi.is_open()) {
		parse_frontmatter(&fi);
		generate_html(&fi);
		fi.close();
		std::ofstream fo;
		fo.open(_out_dir + title + ".html", std::ios::trunc);
		if (fo.is_open()) { fo << html; fo.close(); }
	} else {
		std::cout << "Could not open .md";
	}
};

/* ************************************************************************* */

std::string Article::_root_dir = "";
std::string Article::_src_dir = "";
std::string Article::_style_dir = "";
std::string Article::_out_dir = "";

void Article::set_dir(std::string dir) {
	_root_dir = dir;
	_src_dir = dir + "src/articles/";
	_style_dir = dir + "res/styles/";
	_out_dir = dir + "public/articles/";
}

/* ************************************************************************* */

const std::string Article::FRONTMATTER_DELIM = "--------";
const std::string Article::STR_MDT_SECTION = "#";
const std::string Article::STR_MDT_SUBSECTION = "##";
const std::string Article::STR_MDT_PARAGRAPH = "p";
const std::string Article::STR_MDT_START_NOTE = "[[";
const std::string Article::STR_MDT_SIDENOTE = "#";
const std::string Article::STR_MDT_MARGIN_NOTE = "@";
const std::string Article::STR_MDR_CLOSE_NOTE = "]]";


void Article::parse_frontmatter(std::ifstream *fi) {
	std::string line;
	bool done = 0;
	while (!done && getline(*fi, line)) {
		if (line == FRONTMATTER_DELIM) while (!done && getline(*fi, line)) {
			if (line != FRONTMATTER_DELIM) map_key(line); else done = 1;
		}
	}
};


void Article::map_key(std::string l) {
	std::string key;
	std::string value;
	size_t pos = 0;
	l.erase(std::remove(l.begin(), l.end(), ' '), l.end());
	pos = l.find(":");
	key = l.substr(0, pos);
	l.erase(0, pos + 1);
	value = l;
	switch (resolve_fmk(key)) {
	case FrontmatterKeys::FMK_TIME: {
		time = std::atoi(value.c_str());
		break;
	}
	case FrontmatterKeys::FMK_TITLE: {
		title = value;
		break;
	}
	case FrontmatterKeys::FMK_STYLE: {
		style = value;
		break;
	}
	default: {
		break;
	}
	}
};


FrontmatterKeys Article::resolve_fmk(std::string fmk) {
	if (fmk == "") { return FrontmatterKeys::FMK_INVALID; }
	else if (fmk == "date") { return FrontmatterKeys::FMK_TIME; }
	else if (fmk == "title") { return FrontmatterKeys::FMK_TITLE; }
	else if (fmk == "style") { return FrontmatterKeys::FMK_STYLE; }
	else { return FrontmatterKeys::FMK_INVALID; }
};


void Article::generate_html(std::ifstream *fi) {
	html += "<!DOCTYPE html>\n<html lang=\"en\">\n";
	generate_head();
	html += "<body>\n<article>\n";
	generate_body(fi);
	html += "</article>\n</body>\n";
	html += "</html>\n\n";
};


void Article::generate_head(void) {
	html += \
"<head>\n\
<meta charset=\"utf-8\"/>\n\
<title>" + title + "</title>\n\
<link rel=\"stylesheet\" href=\"" + _style_dir + style + ".css\">\n\
<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">\n\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
</head>\n";
};


void Article::generate_body(std::ifstream *fi) {
	std::string line;
	if (getline(*fi, line)) {
		size_t pos = line.find(' ');
		std::string md = line.substr(0, pos);
		line.erase(0, pos + 1);
		switch (resolve_mdt(md)) {
		case MarkdownTags::MDT_SECTION: {
			generate_section(fi, line);
			break;
		}
		case MarkdownTags::MDT_SUBSECTION: {
			generate_subsection(fi, line);
			break;
		}
		case MarkdownTags::MDT_PARAGRAPH: {
			generate_paragraph(line);
			break;
		}
		case MarkdownTags::MDT_CLOSE_NOTE: {
			std::cout << "How did this happen...?";
			break;
		}
		case MarkdownTags::MDT_INVALID: {
			break;
		}
		default: {
			break;
		}
		}
	} else {
		return;
	}
	generate_body(fi);
};


void Article::generate_section(std::ifstream *fi, std::string l) {
	html += "<section>\n<h1>" + l + "</h1>\n";
	generate_body(fi);
	html += "</section>\n";
};


void Article::generate_subsection(std::ifstream *fi, std::string l) {
	html += "<h2>" + l + "</h2>\n";
	generate_body(fi);
};


void Article::generate_paragraph(std::string l) {
	html += "<p>\n";
	size_t note_start_pos;
	while ((note_start_pos = l.find(STR_MDT_START_NOTE)) != std::string::npos) {
		html += l.substr(0, note_start_pos);
		l.erase(0, note_start_pos + STR_MDT_START_NOTE.length());
		if (l.at(0) == STR_MDT_SIDENOTE.at(0)) {
			l = generate_sidenote(l);
		} else if (l.at(0) == STR_MDT_MARGIN_NOTE.at(0)) {
			l = generate_margin_note(l);
		}
	}
	html += l + "</p>\n";
};


std::string Article::generate_sidenote(std::string l) {
	l.erase(0, 1);
	size_t note_end_pos = l.find(STR_MDR_CLOSE_NOTE);
	std::string note = l.substr(0, note_end_pos);
	l.erase(0, note_end_pos + STR_MDR_CLOSE_NOTE.length());
	std::hash<std::string> hash_note;
	std::string id = std::to_string(hash_note(note));
	html += \
"<label for=\"" + id + "\" class=\"margin-toggle sidenote-number\">\
</label>\n\
<input type=\"checkbox\" id=\"" + id + "\" class=\"margin-toggle\"/>\n\
<span class=\"sidenote\">" + note + "\n</span>\n";
	return l;
};


std::string Article::generate_margin_note(std::string l) {
	l.erase(0, 1);
	size_t note_end_pos = l.find(STR_MDR_CLOSE_NOTE);
	std::string note = l.substr(0, note_end_pos);
	l.erase(0, note_end_pos + STR_MDR_CLOSE_NOTE.length());
	std::hash<std::string> hash_note;
	std::string id = std::to_string(hash_note(note));
	html += \
"<label for=\"" + id + "\" class=\"margin-toggle\">&#8853;</label>\
<input type=\"checkbox\" id=\"" + id + "\" class=\"margin-toggle\"/>\
<span class=\"marginnote\">" + note + "</span>";
	return l;
};


MarkdownTags Article::resolve_mdt(std::string mdt) {
	if (mdt == "") { return MarkdownTags::MDT_INVALID; }
	else if (mdt == STR_MDT_SECTION) { return MarkdownTags::MDT_SECTION; }
	else if (mdt == STR_MDT_SUBSECTION) { return MarkdownTags::MDT_SUBSECTION; }
	else if (mdt == STR_MDT_PARAGRAPH) { return MarkdownTags::MDT_PARAGRAPH; }
	else if (mdt == STR_MDT_START_NOTE) { return MarkdownTags::MDT_START_NOTE; }
	else if (mdt == STR_MDT_SIDENOTE) { return MarkdownTags::MDT_SIDENOTE; }
	else if (mdt == STR_MDT_MARGIN_NOTE) { return MarkdownTags::MDT_MARGIN_NOTE;}
	else if (mdt == STR_MDR_CLOSE_NOTE) { return MarkdownTags::MDT_CLOSE_NOTE;}
	else { return MarkdownTags::MDT_INVALID; }
};





















