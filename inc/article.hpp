#ifndef ARTICLE_HPP
#define ARTICLE_HPP

#include <iostream>

enum FrontmatterKeys {
	FMK_INVALID,
	FMK_TIME,
	FMK_TITLE,
	FMK_STYLE
};

enum MarkdownTags {
	MDT_INVALID,
	MDT_SECTION,
	MDT_SUBSECTION,
	MDT_PARAGRAPH,
	MDT_START_NOTE,
	MDT_SIDENOTE,
	MDT_MARGIN_NOTE,
	MDT_CLOSE_NOTE
};

class Article {
public:
	Article(std::string fp);
	static void set_dir(std::string dir);

private:
	void parse_frontmatter(std::ifstream* fi);
	void map_key(std::string l);
	FrontmatterKeys resolve_fmk(std::string fmk);
	void generate_html(std::ifstream* fi);
	void generate_head(void);
	void generate_body(std::ifstream* fi);
	void generate_section(std::ifstream* fi, std::string l);
	void generate_subsection(std::ifstream* fi, std::string l);
	void generate_paragraph(std::string l);
	std::string generate_sidenote(std::string l);
	std::string generate_margin_note(std::string l);
	MarkdownTags resolve_mdt(std::string mdt);

	std::string html;
	std::string md;

	std::string name;

	std::string title;
	std::string style;
	int time;

	static std::string _root_dir;
	static std::string _src_dir;
	static std::string _style_dir;
	static std::string _out_dir;

	static const std::string FRONTMATTER_DELIM;
	static const std::string STR_MDT_SECTION;
	static const std::string STR_MDT_SUBSECTION;
	static const std::string STR_MDT_PARAGRAPH;
	static const std::string STR_MDT_START_NOTE;
	static const std::string STR_MDT_SIDENOTE;
	static const std::string STR_MDT_MARGIN_NOTE;
	static const std::string STR_MDR_CLOSE_NOTE;
};

#endif // ARTICLE_HPP
