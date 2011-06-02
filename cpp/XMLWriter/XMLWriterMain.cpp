/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <fstream>

#include "XMLWriter.h"

using namespace AWT::XML;

int main(int argc, char** argv)
{
   std::ofstream fout("C:/docume~1/aturner/desktop/style.xsl");
   XMLWriter xmlw(fout, true);

   xmlw.startElement("xsl:stylesheet").addAttribute("version", "1.0").addAttribute("xmlns:xsl", "http://www.w3.org/1999/XSL/Transform");

   xmlw.startElement("xsl:output")  .addAttribute("method", "text")                                            .endElement();
   xmlw.startElement("xsl:param")   .addAttribute("name", "process")      .addAttribute("select", " '*' ")     .endElement();

   xmlw.startElement("xsl:variable").addAttribute("name", "message-dir")  .addAttribute("select", "'messages'").endElement();
   xmlw.startElement("xsl:variable").addAttribute("name", "directory-sep").addAttribute("select", "'/'")       .endElement();
   xmlw.startElement("xsl:variable").addAttribute("name", "include-ext")  .addAttribute("select", "'h'")       .endElement();
   
   xmlw.startElement("xsl:template").addAttribute("name", "toUpper");
   xmlw.startElement("xsl:param")   .addAttribute("name", "x").endElement();
   xmlw.startElement("xsl:value-of").addAttribute("select", "translate($x,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')").endElement();
   xmlw.endElement();

   xmlw.startElement("xsl:template").addAttribute("name", "toLower");
   xmlw.startElement("xsl:param")   .addAttribute("name", "x").endElement();
   xmlw.startElement("xsl:value-of").addAttribute("select", "translate($x,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')").endElement();
   xmlw.endElement();

   xmlw.startElement("xsl:variable").addAttribute("name", "nonsense");
   xmlw.addCData("Hello World - You & I \"mean you no harm!\"]]>").addText("Foo cheese!é\n");
   xmlw.endElement();

   xmlw.startElement("xsl:template").addAttribute("match", "/");

   xmlw.startElement("xsl:text").addText("Original: ").endElement().startElement("xsl:value-of").addAttribute("select", "$nonsense").endElement();

   xmlw.startElement("xsl:text").addText("toLower:  ").endElement();
   xmlw.startElement("xsl:call-template").addAttribute("name", "toLower");
   xmlw.startElement("xsl:with-param")   .addAttribute("name", "x").addAttribute("select", "$nonsense").endElement().endElement();

   xmlw.startElement("xsl:text").addText("toUpper:  ").endElement();
   xmlw.startElement("xsl:call-template").addAttribute("name", "toUpper");
   xmlw.startElement("xsl:with-param")   .addAttribute("name", "x").addAttribute("select", "$nonsense").endElement().endElement();

   xmlw.finish();

   std::cout << std::endl;
}