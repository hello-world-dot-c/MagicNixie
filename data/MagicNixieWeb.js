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
	else if( type === 'bright' )
	{
		var valStr = "BRIGHTNESS=" + val.toString();
	}
	else if( type === 'delay' )
	{
		var valStr = "DELAY=" + val.toString();	
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
	if( button === 'rainbow' )
	{
		var valStr = "SCENE=RAINBOW";
		document.getElementById("activeScene").innerHTML = "Rainbow is active";
		document.getElementById("activeButton").innerHTML = "";
	}
	else if( button === 'static' )
	{
		var valStr = "COLOUR=STATIC";
		document.getElementById("activeButton").innerHTML = "Static is active";
		document.getElementById("activeScene").innerHTML = "";
	}
	else if( button === 'rainbowcycle' ) // Rainbowcycle
	{
		var valStr = "SCENE=RAINBOWCYCLE";
		document.getElementById("activeScene").innerHTML = "Rainbowcycle is active";
		document.getElementById("activeButton").innerHTML = "";
	}
	else if( button === 'fade' )
	{
		var valStr = "COLOUR=FADE";
		document.getElementById("activeButton").innerHTML = "Fade is active";
		document.getElementById("activeScene").innerHTML = "";
	}
	else if( button === 'candle' )
	{
		var valStr = "SCENE=CANDLE";
		document.getElementById("activeScene").innerHTML = "Candle is active";
		document.getElementById("activeButton").innerHTML = "";
		// Slet den anden 
	}
	else if( button === 'save' )
	{
		var valStr = "SAVE=Save configuration";
	}
	client.open("POST", "/", true);
	client.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	client.send(valStr);
}

// Captures the lamp ON/OFF event.
document.addEventListener('DOMContentLoaded', function () 
{
	var checkbox = document.querySelector('input[type="checkbox"]');

	checkbox.addEventListener('change', function () 
	{
		var client = new XMLHttpRequest();		
		if (checkbox.checked) 
		{
			//Turn lamp ON
			var valStr = "LAMP=1";
			document.getElementById("lampStatus").innerHTML = "ON";
		} 
		else 
		{
			//Turn lamp OFF
			var valStr = "LAMP=0";
			document.getElementById("lampStatus").innerHTML = "OFF";			
		}
		client.open("POST", "/", true);
		client.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		client.send(valStr);	
	});
});