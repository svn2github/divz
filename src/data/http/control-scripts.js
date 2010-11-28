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
	if(url == '/go/next')
	{
		if(incSlide(1))
			return;
	}
	else
	if(url == '/go/prev')
	{
		if(incSlide(-1))
			return;
	}
	
	if(window.RequestObject == null) {
		window.RequestObject = xhttp();
	}
	if(window.RequestObject)
	{
		if(url.indexOf('?') <= 0)
			url += '?';
		window.RequestObject.open("GET",url + '&date=' + (new Date().getTime()), true);
		window.RequestObject.send(null);
	}
}

function incSlide(dir)
{
	var elm = null;
	var idx = 0;
	for(var i=0;i<window.imageElementList.length;i++)
	{
		var otherElm = window.imageElementList[i];
		if(otherElm.className == 'live')
		{
			//otherElm.className = 'notlive';
			elm = otherElm;
			idx = i;
		}
	}	
	if(elm != null)// && typeof(elm.onclick) == 'function')
	{
		//elm.className = 'live';
		
		if(dir<0 && idx>0)
		{
			var nextElm = window.imageElementList[idx-1];
			if(typeof(nextElm.onclick))
			{
				nextElm.onclick();
				//nextElm.scrollIntoView(true);
				return true;
			}
		}
		else
		if(dir>0 && idx<window.imageElementList.length-1)
		{
			var nextElm = window.imageElementList[idx+1];
			if(typeof(nextElm.onclick))
			{
				nextElm.onclick();
				//nextElm.scrollIntoView(false);
				return true;
			}
		}
	}
	
	return false;
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
	checkTextForSlide(elm);
}
function checkTextForSlide(elm)
{
	if(!elm)
	{
		//alert("No Elm");
		return;
	}
	var txt = elm.alt;
	if(!txt)
		txt = elm.getAttribute("alt");
	var rx = new RegExp('(\\d) secs');
	var m = rx.exec(txt);
	if(!m)
	{
		//alert("No Match on '"+txt+"'");
		return;
	}
	var sec = m[1];
	var ms = parseInt(sec) * 1000
	//alert(sec+","+ms);
	setTimeout(function(){gotoUrl('/go/next')}, ms);
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