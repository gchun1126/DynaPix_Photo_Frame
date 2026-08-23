#!/usr/bin/env python3
"""Generate PDF documentation from Markdown files."""

import re
import sys
from pathlib import Path

import markdown
import unidecode
from fpdf import FPDF, FontFace, TextStyle

DOCS_DIR = Path(__file__).parent
LOGO_PATH = DOCS_DIR / "dynapix_logo.svg"

FONT_DIR = Path("/System/Library/Fonts/Supplemental")
FONT_FILES = {
    "": FONT_DIR / "Arial.ttf",
    "B": FONT_DIR / "Arial Bold.ttf",
    "I": FONT_DIR / "Arial Italic.ttf",
    "BI": FONT_DIR / "Arial Bold Italic.ttf",
}

TEAL = (22, 128, 138)
DARK = (30, 30, 30)
GRAY = (80, 80, 80)
LIGHT_GRAY = (100, 100, 100)


def make_tag_styles() -> dict[str, TextStyle]:
    """Return compact tag styles to avoid fpdf2's oversized default margins."""
    body = TextStyle(
        font_family="Arial",
        font_size_pt=10,
        color=DARK,
        t_margin=2,
        b_margin=2,
    )
    heading = TextStyle(
        font_family="Arial",
        color=TEAL,
        t_margin=4,
        b_margin=2,
    )
    item = TextStyle(
        font_family="Arial",
        font_size_pt=10,
        color=DARK,
        t_margin=0.5,
        b_margin=0.5,
    )
    styles = {
        "p": body,
        "li": item,
        "ul": item,
        "ol": item,
    }
    for level, size in ((1, 22), (2, 14), (3, 12), (4, 11), (5, 10), (6, 10)):
        styles[f"h{level}"] = TextStyle(
            font_family="Arial",
            font_style="B" if level <= 2 else "",
            font_size_pt=size,
            color=TEAL,
            t_margin=4,
            b_margin=2,
        )
    return styles


class DocPDF(FPDF):
    """Simple themed PDF document."""

    def __init__(self, title: str):
        super().__init__()
        self.doc_title = title
        self.set_auto_page_break(auto=True, margin=12)
        for style, path in FONT_FILES.items():
            if path.exists():
                self.add_font("Arial", style, str(path))
            else:
                print(f"Warning: font file not found: {path}", file=sys.stderr)

    def header(self):
        self.set_font("Arial", "", 8)
        self.set_text_color(*LIGHT_GRAY)
        self.cell(
            0,
            10,
            f"DynaPix Photo Frame -- {self.doc_title}",
            new_x="LMARGIN",
            new_y="NEXT",
        )
        self.ln(2)

    def footer(self):
        self.set_y(-15)
        self.set_font("Arial", "", 8)
        self.set_text_color(*LIGHT_GRAY)
        self.cell(0, 10, f"Page {self.page_no()}", align="C")


def clean_html(html: str) -> str:
    """Make generated HTML friendlier for fpdf2's write_html."""
    # Horizontal rules become section separators, not big blank spaces
    html = re.sub(r"<hr\s*/?>", "", html)
    # Remove empty paragraphs
    html = re.sub(r"<p>\s*</p>", "", html)
    # Nested tags inside <td> are not supported (e.g. <code> from backticks)
    html = re.sub(r"</?code>", "", html)
    # Tighten up multiple consecutive line breaks
    html = re.sub(r"(<br\s*/?>\s*){2,}", "<br/>", html)
    return html


def render_markdown_to_pdf(md_path: Path, pdf_path: Path, title: str):
    md_text = md_path.read_text(encoding="utf-8")
    # fpdf2's core fonts have a limited encoding; map uncommon Unicode
    # characters (e.g. full-width icons, dingbats) to ASCII-safe equivalents.
    md_text = unidecode.unidecode(md_text)

    html = markdown.markdown(md_text, extensions=["extra"])
    html = clean_html(html)

    pdf = DocPDF(title)
    pdf.add_page()

    # Cover page
    if LOGO_PATH.exists():
        pdf.image(str(LOGO_PATH), x=(210 - 45) / 2, y=55, w=45)
        pdf.ln(55)
    else:
        pdf.ln(60)

    pdf.set_font("Arial", "B", 24)
    pdf.set_text_color(*TEAL)
    pdf.multi_cell(0, 16, title, align="C")
    pdf.ln(8)
    pdf.set_font("Arial", "", 12)
    pdf.set_text_color(*GRAY)
    pdf.multi_cell(0, 8, "ESP32-S3 + Waveshare 4\" E6 E-Paper Photo Frame", align="C")
    pdf.ln(16)
    pdf.set_font("Arial", "", 10)
    pdf.multi_cell(0, 6, "Generated from the project repository.", align="C")

    pdf.add_page()

    # Body
    pdf.set_font("Arial", "", 10)
    pdf.set_text_color(*DARK)

    tag_styles = make_tag_styles()
    pdf.write_html(html, font_family="Arial", tag_styles=tag_styles)

    pdf.output(pdf_path)
    print(f"Generated {pdf_path}")


def main():
    files = [
        ("Quick_Setup_Guide.md", "Quick_Setup_Guide.pdf", "Quick Setup Guide"),
        ("Full_User_Manual.md", "Full_User_Manual.pdf", "Full User Manual"),
    ]

    for md_name, pdf_name, title in files:
        md_path = DOCS_DIR / md_name
        pdf_path = DOCS_DIR / pdf_name
        render_markdown_to_pdf(md_path, pdf_path, title)


if __name__ == "__main__":
    main()
