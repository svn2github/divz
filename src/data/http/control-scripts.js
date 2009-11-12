function xhttp()
{
	var req = undefined;
	if (window.XMLHttpRequest)
		req = new XMLHttpRequest();
	else if (window.ActiveXObject)
		req = new ActiveXObject("Microsoft.XMLHTTP");
	return req;
}
window.imageElementList = [];
function gotoUrl(url)
{
	if(window.RequestObject == null) {
		window.RequestObject = xhttp();
	}
	if(window.RequestObject)
	{
		window.RequestObject.open("GET",url, true);
		window.RequestObject.send(null);
	}
}
function setSlide(url,elm)
{
	gotoUrl(url);
	for(var i=0;i<window.imageElementList.length;i++)
	{
		var otherElm = window.imageElementList[i];
		if(otherElm.className == 'live')
			otherElm.className = 'notlive';
	}	
	elm.className = 'live';
}
window.CurrentBlackState = null;
window.CurrentClearState = null;
function toggle_black(initialState,elm)
{
	if(window.CurrentBlackState == null)
		window.CurrentBlackState = initialState;
	
	window.CurrentBlackState = window.CurrentBlackState ? 0 : 1;
	
	var url = '/toggle/black/' + window.CurrentBlackState;
	gotoUrl(url);
	
	elm.className = window.CurrentBlackState ? 'ctrl_enabled' : 'ctrl_disabled';
}
function toggle_clear(initialState,elm)
{
	if(window.CurrentClearState == null)
		window.CurrentClearState = initialState;
	
	window.CurrentClearState = window.CurrentClearState ? 0 : 1;
	
	var url = '/toggle/clear/' + window.CurrentClearState;
	gotoUrl(url);
	
	elm.className = window.CurrentClearState ? 'ctrl_enabled' : 'ctrl_disabled';
}