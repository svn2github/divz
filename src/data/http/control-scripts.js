function encode(x)
{
	return encodeURI(x)
		.replace(/#/g,'%23')
		.replace(/&/g,'%26')
		.replace(/@/g,'%40')
		.replace(/;/g,'%3b');
}
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
window.CurrentQSlideState = null;
function toggle_qslide(initialState,elm)
{
	if(!elm) // called from if_enter() probably
		elm = document.getElementById('quickslide_link');
		
	if(window.CurrentQSlideState == null)
		window.CurrentQSlideState = initialState;
	
	window.CurrentQSlideState = window.CurrentQSlideState ? 0 : 1;
	
	var url = '/toggle/qslide/' + window.CurrentQSlideState + '?text=' + encode(document.getElementById('qslide').value);
	gotoUrl(url);
	
	elm.className = window.CurrentQSlideState ? 'ctrl_enabled' : 'ctrl_disabled';
}

function if_enter(e,js)
{
	if (!e) var e = window.event;
	var code;
	if (e.keyCode) code = e.keyCode;
	else if (e.which) code = e.which;
	if(code == 13)
		eval(js);
}