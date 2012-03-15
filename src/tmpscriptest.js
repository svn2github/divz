// Global funcs: debug(...), findTextItem(slide,name)
// Global vars:
// dSlide, dGroup, dTemplateGroup, dFormat
// if dFormat is true, then dText is present
// if dFormat is false, then dVars is present
// newSlideCreated(slideNumber) is called for each slide added with those global vars already set

/* This script is designed to be used with a textfile of trvia questions in the format of:

	#text: In what book does it say, “There is a time for everything”?
	#opt1: A. Ecclesiastes*
	#opt2: B. Isaiah
	#opt3: C. Lamentations
	#opt4: D. The Book of Life
	#opt5: E. A and D

And a template designed with the above named fields (well, six #optX fields anyway).
 
*/ 

function numOpts()
{
	for(var i=1; i<=6; i++)
	{
		var valName = "#opt"+i;
		if(!dVars[valName])
			return i-1;	
	}
	return 6;
}

function newSlideCreated(slideNum)
{
	debug("newSlideCreated(",slideNum,"): Start of function, dSlide:", dSlide, ", dFormat:", dFormat, ", dVars:", dVars);

	var num = numOpts(); 
	debug("newSlideCreated(",slideNum,"): num opts:",num);
	
	if(num == 4)
	{
		// move opts 3 and 4 to 4 and 5 to bump them into the next column symetrically
		dVars["#opt5"] = dVars["#opt4"];
		dVars["#opt4"] = dVars["#opt3"];
		delete dVars["#opt3"];
	}
	
	// Find the "answer"
	var answerOpt = false;
	for(var i=1; i<=6; i++)
	{
		var valName = "#opt"+i;
		var str = dVars[valName];
		if(!answerOpt &&
		   str && str.indexOf("*") > -1)
		   {
		   	dVars[valName] = str.replace("*","");
		   	answerOpt = valName;
		   }
	}
	
	if(answerOpt)
	{
		var answerBox = findTextItem(dSlide, answerOpt);
		if(answerBox)
		{
			debug("- found answer ",answerOpt, ", updating slide");
			
			//answerBox.zRotation = 90;
			answerBox.zoomEffectEnabled = true;
		}
		else
			debug("- answer is ",answerOpt,", but item not found on slide");
	}
	else
	{
		debug("- no answer found on this slide");
	}
}