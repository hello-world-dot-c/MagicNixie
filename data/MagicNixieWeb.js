/*
  Scriptfiles for IKEA hack file: Bootstrap_grid_1.html
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
	document.getElementById("square").style.backgroundColor = getRandomColor();
}

function mySliderFunction(type, val) 
{
	var client = new XMLHttpRequest();
	if( type === 'colour' )
	{
		var valStr = "RED=" + red_slider.value + "&GREEN=" + green_slider.value + "&BLUE=" + blue_slider.value;
		// Skal skrives sådan: "#00FF00"
		// Skal huske at konverter til Number, ellers fungerer toString(16) ikke.		
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
	// Skal skrives sådan: "#00FF00"
	// Skal huske at konverter til Number, ellers fungerer toString(16) ikke.
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
		var valStr = "SCEENE=RAINBOW";
		document.getElementById("activeSceene").innerHTML = "Rainbow is active";
		document.getElementById("activeButton").innerHTML = "";
	}
	else if( button === 'static' )
	{
		var valStr = "COLOUR=STATIC";
		document.getElementById("activeButton").innerHTML = "Static is active";
		document.getElementById("activeSceene").innerHTML = "";
	}
	else if( button === 'rainbowcycle' ) // Rainbowcycle
	{
		var valStr = "SCEENE=RAINBOWCYCLE";
		document.getElementById("activeSceene").innerHTML = "Rainbowcycle is active";
		document.getElementById("activeButton").innerHTML = "";
	}
	else if( button === 'fade' )
	{
		var valStr = "COLOUR=FADE";
		document.getElementById("activeButton").innerHTML = "Fade is active";
		document.getElementById("activeSceene").innerHTML = "";
	}
	else if( button === 'candle' )
	{
		var valStr = "SCEENE=CANDLE";
		document.getElementById("activeSceene").innerHTML = "Candle is active";
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