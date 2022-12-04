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
	document.getElementById("colorBox").style.backgroundColor = hexFromRGB( Number(red_slider.value), Number(green_slider.value), Number(blue_slider.value) );
}

function mySliderFunction(type, val) 
{
	var client = new XMLHttpRequest();
	if( type === 'colour' )
	{
		var valStr = "RED=" + red_slider.value + "&GREEN=" + green_slider.value + "&BLUE=" + blue_slider.value;
		// Must be written like this: "#00FF00"
		// Must remember to convert to number, otherwise toString (16) will not work.		
		document.getElementById("colorBox").style.backgroundColor = hexFromRGB( Number(red_slider.value), Number(green_slider.value), Number(blue_slider.value) );		
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
	else if( button === 'reset' )
	{
		var valStr = "RESET=Reset clock";
	}
	client.open("POST", "/", true);
	client.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	client.send(valStr);
}

// Captures the switches ON/OFF events.
document.addEventListener('DOMContentLoaded', function () 
{
	var checkboxes = document.querySelectorAll('input[type="checkbox"]');

    for (var i = 0; i < checkboxes.length; i++) {
        checkboxes[i].addEventListener('change', function (event) 
        {
            var valStrNum = "";
            var valStr = "";
            if (this.checked) 
            {
                //Turn switch ON
                valStrNum = "1";
                valStr = "ON";
            } 
            else 
            {
                //Turn switch OFF
                valStrNum = "0";
                valStr = "OFF";			
            }
            var varStrNum = "";
            var varStr = "";
            if (event.target.id === 'blendingOnOffButton')
            {
                varStrNum = "BLEND";
                varStr = "blendingStatus";
            }
            if (event.target.id === 'show12hDisplayButton')
            {
                varStrNum = "USE12HDISP";
                varStr = "show12hStatus";
            }
            if (event.target.id === 'hoursLeadingZeroesButton')
            {
                varStrNum = "SHOWLEAD0HR";
                varStr = "leadingZeroesStatus";
            }
            if (event.target.id === 'dateLeadingZeroesButton')
            {
                varStrNum = "SHOWLEAD0DT";
                varStr = "dateLeadingZeroesStatus";
            }
            if (event.target.id === 'quietNightsButton')
            {
                varStrNum = "QUIETNIGHTS";
                varStr = "quietNightsStatus";
            }
            valStrNum = varStrNum + "=" + valStrNum;
            document.getElementById(varStr).innerHTML = valStr;
            var client = new XMLHttpRequest();		
            client.open("POST", "/", true);
            client.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            client.send(valStrNum);
        });
    }
});
