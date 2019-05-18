var menuList = new Array(
	"", 			      0,		1, 	     str_menu.InterfaceMangement,
	"if_basic.cgi", 	      0,		2,  	 str_menu.if_basic,		
	"if_l3.cgi",              0,		2,  	 str_menu.if_l3
);                            

var map = new Array();

function menuInit(option)
{
	for (var i = 0; i < option.length; i++)
	{
		if ( option[i] == 0 )
		{
			continue;
		}
		for (var n = 0; n < menuList.length; n +=4 )
		{
			if ( menuList[n] == option[i] )
			{
				menuList[n+1] = 1;
				break;
			}
		}
	}
	n = menuList.length - 4;
	var url = "";
	var level = 0;
	while (n >= 0)
	{
		if ( menuList[n+1] == 1 && menuList[n+2] > 0)
		{
			url = menuList[n];
			level = menuList[n+2];
		}
		else if ( menuList[n+2] > 0 && menuList[n+2] < level )
		{
			menuList[n] = url;
			menuList[n+1] = 1;
			level = menuList[n+2];
		}
		n -= 4;
	}
}

function menuDisplay()
{
	var i = 0;
	var className;
	for (var n = 0; n < menuList.length; n +=4  )
	{
		if ( menuList[n+1] != 1 )
		{
			continue;
		}
		if (menuList[n+2] == 0)
		{
			className = "dot1";
			display = "block";
		}
		else if ( (menuList[n+2] > 0) && (menuList[n+4+2] > menuList[n+2]) )
		{
			className = "plus";
			if ( menuList[n+2] == 1 )
			{
				display = "block";
			}
			else
			{
				display = "none";
			}
		}
		else
		{
			className = "dot2";
			display = "none";
		}
		var power = (menuList[n+2] > 0)?(menuList[n+2] - 1):0;
		document.write('<ol id=ol'+i+' class='+className+' style="display:'+display+'; background-position:'+(13*power)+'px 3px;PADDING-LEFT: '+(13*power+13)+'px;"><A id=a'+i+' href="./'+menuList[n]+'" target=mainFrame class=L1 onClick="doClick('+i+');">'+menuList[n+3]+'</a></ol>');
		map[map.length] = menuList[n+2];
		i++;
	}
}

function collapseAll()
{
	var e;
	for(var i=0;;i++)
	{
		try{
			if(map[i] > 1)
			{
				document.getElementById('ol'+i).style.display = "none";
			}
			if(document.getElementById('ol'+i).className == "minus")
			{
				document.getElementById('ol'+i).className = "plus";
			}
		}
		catch(e)
		{
			break;
		}
	}
	for(var i=0;i<document.links.length;i++)
	{
		document.links[i].className = "L1";
	}
}

function expandBranch(n)
{
	var branch;
	var l = 0;
	var index;
	while( l != 1 )
	{
		branch = document.getElementById('ol'+n);
		l = map[n];
		index = n;
		if (branch.className == "plus")
		{
			branch.className = "minus";
		}
		else
		{
			while(1)
			{
				if (map[index] != l - 1)
					index--;
				else 
					break;
			}
			branch = document.getElementById('ol'+index);
			branch.className = "minus";
		}
		n = index;
		l = map[n];
		while(1)
		{
			index++;
			if(index >= map.length)
			{
				break;
			}
			branch = document.getElementById('ol'+index);
			if (map[index] == (l + 1))
			{
				branch.style.display = "block";
			}
			else if (map[index] <= l)
			{
				break;
			}
		}
	}
}

function doClick(n)
{
	var e;
	collapseAll();
	obj = document.getElementById('ol'+n);
	if (obj.className == "plus")
	{
		document.getElementById('a'+(n+1)).className = "L2";
	}
	else
	{
		document.getElementById('a'+n).className = "L2";
	}
	if (map[n] > 0)
		expandBranch(n);
}
