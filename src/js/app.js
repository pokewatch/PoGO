var myLatitude, myLongitude;
var pkmnLatitude, pkmnLongitude;

var distance, bearing;

Pebble.addEventListener("ready", function(e){
	getLocation();
	getPokemon();
});

Pebble.addEventListener("appmessage", function(e){
	getPokemon();
});

function getLocation(){
	if(navigator && navigator.geolocation){
		navigator.geolocation.watchPosition(
			function(pos){ //Success - High Acc
				myLatitude = pos.coords.latitude; 
				myLongitude = pos.coords.longitude;
			},
			function(pos){ //Fail - High Acc
				navigator.geolocation.watchPosition(
					function(pos){ //Success - Low Acc
						myLatitude = pos.coords.latitude;
						myLongitude = pos.coords.longitude;
					},
					function(pos){ //Fail - Low Acc
						Pebble.showSimpleNotificationOnPebble("GPS Error", "Unable to detect location - please check your phone to ensure GPS is enabled.");
					},
					{
						maximumAge:600000, 
						timeout:10000, 
						enableHighAccuracy: false
					}
				);																																					
			},
			{
				maximumAge:600000, 
				timeout:5000, 
				enableHighAccuracy: true}
		);
	}
	else{
		Pebble.showSimpleNotificationOnPebble("GPS Disabled", "Unable to detect location - please check your phone to ensure GPS is enabled.");
	}
}

function getPokemon(){
	var xhr = new XMLHttpRequest();
	var url = "https://pokevision.com/map/data/" + myLatitude + "/" + myLongitude;
	xhr.open("GET", url, false);
	xhr.send();
	
	var result = JSON.parse(xhr.responseText);
	
	if(result.status === "success"){
		
	}
	else{
		Pebble.showSimpleNotificationOnPebble("API Failed", "Could not call Pokevision API");
	}
}