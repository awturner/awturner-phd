
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <fstream>

#include "XMLWriter.h"

using namespace AWT::XML;

int main( int argc, char** argv )
{
   std::ofstream fout("C:/docume~1/aturner/desktop/style.xsl");
   XMLWriter xmlw( fout, true );

   xmlw.startElement("xsl:stylesheet").addAttribute("version", "1.0").addAttribute("xmlns:xsl", "http://www.w3.org/1999/XSL/Transform");

   xmlw.startElement("xsl:output")  .addAttribute("method", "text")                                            .endElement( );
   xmlw.startElement("xsl:param")   .addAttribute("name", "process")      .addAttribute("select", " '*' ")     .endElement( );

   xmlw.startElement("xsl:variable").addAttribute("name", "message-dir")  .addAttribute("select", "'messages'").endElement( );
   xmlw.startElement("xsl:variable").addAttribute("name", "directory-sep").addAttribute("select", "'/'")       .endElement( );
   xmlw.startElement("xsl:variable").addAttribute("name", "include-ext")  .addAttribute("select", "'h'")       .endElement( );
   
   xmlw.startElement("xsl:template").addAttribute("name", "toUpper");
   xmlw.startElement("xsl:param")   .addAttribute("name", "x").endElement( );
   xmlw.startElement("xsl:value-of").addAttribute("select", "translate($x,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')").endElement( );
   xmlw.endElement( );

   xmlw.startElement("xsl:template").addAttribute("name", "toLower");
   xmlw.startElement("xsl:param")   .addAttribute("name", "x").endElement( );
   xmlw.startElement("xsl:value-of").addAttribute("select", "translate($x,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')").endElement( );
   xmlw.endElement( );

   xmlw.startElement("xsl:variable").addAttribute("name", "nonsense");
   xmlw.addCData("Hello World - You & I \"mean you no harm!\"]]>").addText("Foo cheese!é\n");
   xmlw.endElement( );

   xmlw.startElement("xsl:template").addAttribute("match", "/");

   xmlw.startElement("xsl:text").addText("Original: ").endElement( ).startElement("xsl:value-of").addAttribute("select", "$nonsense" ).endElement( );

   xmlw.startElement("xsl:text").addText("toLower:  ").endElement( );
   xmlw.startElement("xsl:call-template").addAttribute("name", "toLower");
   xmlw.startElement("xsl:with-param")   .addAttribute("name", "x").addAttribute("select", "$nonsense").endElement( ).endElement( );

   xmlw.startElement("xsl:text").addText("toUpper:  ").endElement( );
   xmlw.startElement("xsl:call-template").addAttribute("name", "toUpper");
   xmlw.startElement("xsl:with-param")   .addAttribute("name", "x").addAttribute("select", "$nonsense").endElement( ).endElement( );

   xmlw.finish( );

   std::cout << std::endl;
}