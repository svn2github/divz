// Global funcs: debug(...), findTextItem(slide,name)
// Global vars:
// dSlide, dGroup, dTemplateGroup, dFormat
// if dFormat is true, then dText is present
// if dFormat is false, then dVars is present
// aboutToCreateSlide(slideNumber) is called for each slide added with those global vars already set

/* This script is designed to be used with a textfile of trvia questions and apply pre/post formatting
   to the slide templates.
 
*/

function min(a,b) { return a<b?a:b; }
function max(a,b) { return a>b?a:b; }

var tmpPtr = {opt1:false,opt2:false,foundAnswer:false};

// First hook for the import dialog, called just before creating the slide, giving us an opportunity to change
// the variables read from the text file.
function aboutToCreateSlide(slideNum)
{
	debug("aboutToCreateSlide(",slideNum,"): Start of function, dSlide:", dSlide, ", dFormat:", dFormat, ", dVars:", dVars);

	tmpPtr.foundAnswer = false;
	
	// Convert to HTML so we can apply highlighting of the answer
	dVars["#opt1"] = processOption(dVars["#opt1"]);
	dVars["#opt2"] = processOption(dVars["#opt2"]);
	
	dSlide.autoChangeTime = tmpPtr.foundAnswer ? 8. : 17.;
	dSlide.slideName      = tmpPtr.foundAnswer ? "Q "+slideNum+" - ANSWER" : "Q "+(slideNum+1);
	
	// store to equilize font sizes after this slide is created
	tmpPtr.opt1 = findTextItem(dSlide,"#opt1");
	tmpPtr.opt2 = findTextItem(dSlide,"#opt2");
	
}

// The second hook, called just after the slide is created and variable values are applied. At this point,
// the contents of the blocks have been fitted to the rectangles and everything is added to the slide group.
function newSlideCreated(slideNum)
{
	//debug("newSlideCreated(",slideNum,"): Start of function, dSlide:", dSlide, ", tmpPtr.opt1:", tmpPtr.opt1, ", tmpPtr.opt2:",tmpPtr.opt2);
	
	if(tmpPtr.opt1 && tmpPtr.opt2)
	{
		// Find font size of each of the otpion blocks
		var fontSize1 = findFontSize(tmpPtr.opt1);
		var fontSize2 = findFontSize(tmpPtr.opt2);
		
		// Sometimes findFontSize reports 0, so we use the non-zero font size (we assume)
		fontSize1 = fontSize1 == 0 ? fontSize2 : fontSize1;
		fontSize2 = fontSize2 == 0 ? fontSize1 : fontSize2;
		
		// Get the lowest font size of the two blocks
		var minFont = min(fontSize1,fontSize2);
		minFont = max(32,minFont * .95); // reduce the font size used by 5% to ensure we stay within bounds
		
		//debug("newSlideCreated(",slideNum,"): Using min font:",minFont,", f1:",fontSize1,", f2:", fontSize2);
		
		// Change both text boxes to have the same font size
		changeFontSize(tmpPtr.opt2, minFont);
		changeFontSize(tmpPtr.opt1, minFont);
	}
	
	return true;
}

// HTML values for use in converting the plain text to Qt-parsable rich text

// Block header/footer wraps the entire block of text
var blockHeader = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
		+ "<html>"
		+ "<head><meta name=\"qrichtext\" content=\"1\" />"
		+ "<style type=\"text/css\">p, li { white-space: pre-wrap; }</style>"
		+ "</head>"
		+ "<body style=\"font-family:'Calibri'; font-size:32pt; font-weight:400; font-style:normal;\">";
var blockFooter	= "</body></html>";

// Line prefix/suffix wraps each line and applies the font family (face), font size, and font weight
var linePrefix	= "<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
		+ "<span style=\" font-family:'Calibri'; font-size:32pt; font-weight:800;\">"; // 32 is minimum font size (will be adjusted in importer to fit text rect), 800 weight is bold
var lineSuffix	= "</span></p>";

// Wrap line prefix/suffix with ansPrefix/suffix to provide a highlight for the answer block - no font formatting is provided here though
var ansPrefix	= "<table border=\"1\" style=\" border-color:#000000; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\" cellspacing=\"0\" cellpadding=\"0\" bgcolor=\"#ffff00\"><tr><td>";
var ansSuffix	= "</td></tr></table>";

// Convert the plain text in 'block' to a set of HTML paragraphs in a strictly compliant HTML document, suitable for use in Qt's rich text document 
function processOption(block)
{
	if(!block)
		return "";
		
	var html = blockHeader;

	// Split the block into a list of lines,
	// the go thru each line and create an HTML
	// paragraph using linePrefix/lineSuffix.
	//
	// If the line is marked with an asterisk, 
	// then highlight the line as the 'answer'
	// using ansPrefix and ansSuffix around 
	// linePrefix/lineSuffix to give it a yellow
	// background highlight.
	
	var lines = block.split("\n");
	for(var i=0; i<lines.length; i++)
	{
		var line = lines[i];
		if(line)
		{
			var isAnswer = line.indexOf("*") > -1;
			if(isAnswer)
			{
				html += ansPrefix;
				line = line.replace("*","");
				tmpPtr.foundAnswer = true;
			}
			
			html += linePrefix
			      + line
			      + lineSuffix;
			
			if(isAnswer)
				html += ansSuffix;
		}
	}
	
	html += blockFooter;
	
	return html;
}
