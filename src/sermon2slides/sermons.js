// Global funcs: debug(...), findTextItem(slide,name)
// Global vars:
// dSlide, dGroup, dTemplateGroup, dFormat
// if dFormat is true, then dText is present
// if dFormat is false, then dVars is present
// aboutToCreateSlide(slideNumber) is called for each slide added with those global vars already set

/* This script handles two things:
	- Selecting the proper slide template based on the contents of the current slide
	- Inserting Bible verse refs into the group
*/

var refToAdd = "";
var lastNum = "";

// First hook for the import dialog, called just before creating the slide, giving us an opportunity to change
// the variables read from the text file.
function aboutToCreateSlide(slideNum)
{
}

// The second hook, called just after the slide is created and variable values are applied. At this point,
// the contents of the blocks have been fitted to the rectangles and everything is added to the slide group.
function newSlideCreated(slideNum)
{
	if(dVars["#point"])
	{
		lastNum = (dVars["#num"] ? dVars["#num"]+". " : "");
		dSlide.slideName = lastNum + dVars["#point"];
	}
	else
	if(dVars["#define"])
		dSlide.slideName = lastNum + "DEF-"+ (dVars["#word"] ? dVars["#word"] : dVars["#define"]);
	else
	if(dVars["#bible"])
		dSlide.slideName = lastNum + "Bible - "+dVars["#bible"];
	else
	if(dVars["#title"])
		dSlide.slideName = "Title Slide";
	

	if(refToAdd)
	{
		if(InPrimaryGroup)
		{
			BibleBrowser.setShowEachVerseOnSeperateSlide(true);
			BibleBrowser.setAppendToExistingGroup(true);
			BibleBrowser.addVersesToGroup(refToAdd, dGroup);
			debug("Added bible reference: ", refToAdd);
		}
// 		else
// 			debug("Not adding ref ", refToAdd, " because not in primary group");
	}
}

// This hook allows you to return the name of a slide from the template group to find.
// If the name given is not found, it will try to find a slide by the current slide num.
function getTemplateSlideName()
{
	if(dVars["#point"])
		return "#point";
	
	if(dVars["#define"])
		return "#define";
	
	if(dVars["#bible"])
	{
		// Ref will be added by newSlideCreated(*), above
		refToAdd = dVars["#bible"];
		return "#bible";
	}
	
	if(dVars["#title"])
		return "#title";
}



