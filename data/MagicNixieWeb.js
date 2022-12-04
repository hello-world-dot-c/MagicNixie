/*
  Scriptfile for MagicNixie file: MagicNixieWeb.html
*/
function hexFromRGB(r, g, b) 
{

    var hex = [
      r.toString( 16 ),
      g.toString( 16 ),
      b.toString( 16 )
    ];
	
    $.each( hex, function( nr, val ) 
	{
      if ( val.length === 1 ) 
	  {
        hex[ nr ] = "0" + val;
      }
    });
    return "#"+hex.join( "" ).toUpperCase();
}

function getRandomColor() 
{
	var letters = "0123456789abcdef";
	var result = "#";
	for (var i = 0; i < 6; i++) 
	{
		result += letters.charAt(parseInt(Math.random() * letters.length));
	}
	return result;
}

function changeColors() 
{
	document.getElementById("square").style.backgroundColor = hexFromRGB( Number(red_slider.value), Number(green_slider.value), Number(blue_slider.value) );
}

function mySliderFunction(type, val) 
{
	var client = new XMLHttpRequest();
	if( type === 'colour' )
	{
		var valStr = "RED=" + red_slider.value + "&GREEN=" + green_slider.value + "&BLUE=" + blue_slider.value;
		// Must be written like this: "#00FF00"
		// Must remember to convert to number, otherwise toString (16) will not work.		
		document.getElementById("square").style.backgroundColor = hexFromRGB( Number(red_slider.value), Number(green_slider.value), Number(blue_slider.value) );		
	}
	else if( type === 'ledbright' )
	{
		var valStr = "BRIGHTNESS=" + val.toString();
	}
	else if( type === 'nixiebright' )
	{
		var valStr = "NIXIEBRIGHTNESS=" + val.toString();
	}
	else if( type === 'antipoison' )
	{
		var valStr = "ANTIPOISON=" + val.toString();	
	}
	// Must be written like this: "#00FF00"
	// Must remember to convert to number, otherwise toString (16) will not work.
  //document.getElementById("square").style.backgroundColor = hexFromRGB( Number(red_slider.value), Number(green_slider.value), Number(blue_slider.value) );
	client.open("POST", "/", true);
	client.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	client.send(valStr);
}

// Captures all button activations.
function buttonSubmit(button)
{ 
	var client = new XMLHttpRequest();
	if( button === 'save' )
	{
		var valStr = "SAVE=Save configuration";
	}
	client.open("POST", "/", true);
	client.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	client.send(valStr);
}

// Captures the blending ON/OFF event.
document.addEventListener('DOMContentLoaded', function () 
{
	var checkbox = document.querySelector('input[type="checkbox"]');

	checkbox.addEventListener('change', function () 
	{
		var client = new XMLHttpRequest();		
		if (checkbox.checked) 
		{
			//Turn blending ON
			var valStr = "BLEND=1";
			document.getElementById("blendingStatus").innerHTML = "ON";
		} 
		else 
		{
			//Turn blending OFF
			var valStr = "BLEND=0";
			document.getElementById("blendingStatus").innerHTML = "OFF";			
		}
		client.open("POST", "/", true);
		client.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		client.send(valStr);	
	});
});