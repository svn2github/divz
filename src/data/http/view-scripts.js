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
function gotoUrl(url)
{
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

function serverResponse()
{
	if(window.RequestObject &&
	   window.RequestObject.readyState == 4 &&
	   window.RequestObject.status     == 200)
	{
		var response;
		eval('response=' + window.RequestObject.responseText);
		//alert(response+'|' + window.RequestObject.responseText);
		if(response.no_change)
		{
			// do nothing
		}
		else
		{
			window.CurrentSlideID = response.slide_id;
			window.CurrentSlideName = response.slide_name;
			
			var elm = document.getElementById('doc_title');
			if(elm) elm.innerHTML = response.doc_name;
			
			var elm = document.getElementById('slide_name');
			if(elm) elm.innerHTML = response.slide_name;
			
			var elm = document.getElementById('group_name');
			if(elm) elm.innerHTML = response.group_name;
			
			var elm = document.getElementById('view_image');
			if(elm) elm.src = '/image?date=' + (new Date().getTime());
			
		}
	}
}

function pollServer()
{
	if(window.RequestObject == null) 
	{
		window.RequestObject = xhttp();
	}
		
	if(window.RequestObject)
	{
		window.RequestObject.onreadystatechange = serverResponse;
		window.RequestObject.open("GET",'/poll?slide_id=' + window.CurrentSlideID + 
							'&slide_name=' + encode(window.CurrentSlideName) + 
							'&date=' + (new Date().getTime()), true);
		window.RequestObject.send(null);
	}
}

function startPolling()
{
	window.setInterval(pollServer,500);
}
