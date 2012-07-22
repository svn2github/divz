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
var textBoxPtr = false;
var lastVerseAdded = "";

debug("Initializing sermon.js script");

var files = getFileList("./sermon2slides/backgrounds/", "*.jpg");
var rval = Math.random();
var randIdx = parseInt(rval * files.length);
//debug("rval:",rval," length:",files.length,", randIdx:",randIdx);
var backgroundFile = "./sermon2slides/backgrounds/" + files[randIdx];
debug("Using background: ", backgroundFile);

// First hook for the import dialog, called just before creating the slide, giving us an opportunity to change
// the variables read from the text file.
function aboutToCreateSlide(slideNum)
{
	if(InPrimaryGroup)
		// Yes, this will set the group name multiple times - but until we add a new 'newGroupCreated' API or whatever,
		// this is where we'll put it
		dGroup.groupTitle = "Message Slides";
		
	textBoxPtr = false;
	
	if(dVars["#bible"])
	{
		// Ref will be added by newSlideCreated(), below
		refToAdd = dVars["#bible"];
		// Used to adjust the size of the box later
		textBoxPtr = findTextItem(dSlide,"#bible");
	}
	else
	if(dVars["#point"])
		textBoxPtr = findTextItem(dSlide,"#point");
	else
	if(dVars["#title"])
		textBoxPtr = findTextItem(dSlide,"#title");
	
}

// The second hook, called just after the slide is created and variable values are applied. At this point,
// the contents of the blocks have been fitted to the rectangles and everything is added to the slide group.
function newSlideCreated(slideNum)
{
	if(textBoxPtr)
		intelligentCenterTextbox(textBoxPtr);
	
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
	
	if(InPrimaryGroup)
		changeSlideBackground(dSlide, backgroundFile);

	if(refToAdd)
	{
		if(InPrimaryGroup)
		{
			// Check the last verse added so we dont duplicate verses right next to each other
			if(lastVerseAdded != refToAdd)
			{
				BibleBrowser.setShowEachVerseOnSeperateSlide(true);
				//BibleBrowser.setAppendToExistingGroup(true);
				
				// Leaving the second arg off (e.g. NULL) will cause 
				// BibleBrowser to add the verses as a group to the current document instead of dGroup
				BibleBrowser.addVersesToGroup(refToAdd);
				//BibleBrowser.addVersesToGroup(refToAdd, dGroup);
				
				debug("Added bible reference: ", refToAdd);
				
				//BibleBrowser.setAppendToExistingGroup(false);
			}
			
			lastVerseAdded = refToAdd;
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
		return "#bible";
	
	if(dVars["#title"])
		return "#title";
}



