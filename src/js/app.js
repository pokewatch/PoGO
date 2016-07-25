//"use strict";

// XMLHttpRequest helper
var xhrRequest = function (url, type, callback) {
	var xhr = new XMLHttpRequest();
	xhr.onload = function () {
		callback(this.responseText);
	};
	xhr.open(type, url);
	xhr.send();
};

function getPokemon(latitude, longitude) {

	// live PokeVision data, hard-coded to Ann Arbor for now
	var scanUrl = 'https://pokevision.com/map/scan/' + latitude + '/' + longitude;
	var dataUrl = 'https://pokevision.com/map/data/' + latitude + '/' + longitude;

	// static (stable!) example of PokeVision data
	//var scanUrl = 'https://mathewreiss.github.io/PoGO/data.json';
	//var dataUrl = 'https://mathewreiss.github.io/PoGO/data.json';

	// TODO: is this OK?
	xhrRequest(scanUrl, 'GET', 
		function(scanResponseText) {

			// TODO: need to check for "Maintenance..." page - may not even be JSON!

			var scanJson = JSON.parse(scanResponseText);
			console.log(scanResponseText); // JSON.stringify() not necessary!

			// TODO: check scanResponseText success (although...does throttling error matter
			// since we can still view pokes from last scan...?

			xhrRequest(dataUrl, 'GET', 
				function(dataResponseText) {
					var json = JSON.parse(dataResponseText);
					console.log(dataResponseText); // JSON.stringify() not necessary!

					// TODO: status check!
					console.log('status is "' + json.status + '"');

					// TODO: much better error checking???
					if (json.pokemon.length > 0) {
						console.log('pokemon[0].pokemonId is "' + json.pokemon[0].pokemonId + '"');
						// PokeVision is string for some reason
						var pokemonId = Number(json.pokemon[0].pokemonId);
						console.log('pokemonId is "' + pokemonId + '"');

						var pokemonExpirationTime = json.pokemon[0].expiration_time;
						console.log('pokemonExpirationTime is "' + pokemonExpirationTime + '"');

						var pokemonLatitude = json.pokemon[0].latitude;
						console.log('pokemonLatitude is "' + pokemonLatitude + '"');
						var pokemonLongitude = json.pokemon[0].longitude;
						console.log('pokemonLongitude is "' + pokemonLongitude + '"');

						var pokemonDistance = getDistance(latitude, longitude, pokemonLatitude, pokemonLongitude);

						// Assemble dictionary using our keys
						var dictionary = {
							"PokemonId": pokemonId,
							"PokemonExpirationTime": pokemonExpirationTime,
							"PokemonDistance": pokemonDistance
						};

						// Send to Pebble
						Pebble.sendAppMessage(dictionary,
							function(e) {
								console.log("AppMessage sent to Pebble successfully!");
							},
							function(e) {
								console.log("Error sending AppMessage to Pebble!");
							}
						);
					} else {
						// no pokemon found!
						Pebble.showSimpleNotificationOnPebble("No Pokemon found!", "(" + latitude + ", " + longitude + ")");
					}


				}
			);

		}
	);

}


function geolocationSuccess(pos) {
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);

  getPokemon(pos.coords.latitude, pos.coords.longitude);
}

function geolocationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);

  // TODO: alert user via watchapp instead
  Pebble.showSimpleNotificationOnPebble("location error", err.message);
}

function updateLocation() {
	var options = {
	  enableHighAccuracy: true,
	  maximumAge: 10000,
	  timeout: 10000
	};

	navigator.geolocation.getCurrentPosition(geolocationSuccess, geolocationError, options);
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
	function(e) {
		console.log('PebbleKit JS ready!');

		updateLocation();
	}
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
	function(e) {
		console.log('AppMessage received!');

		updateLocation();
	}                     
);


// based on @mathew's process_distance()
function getDistance(myLatitude, myLongitude, pkmnLatitude, pkmnLongitude) {
	var distance;

	var lat1 = myLatitude, lon1 = myLongitude;
	var lat2 = pkmnLatitude, lon2 = pkmnLongitude;
	
	var dLat = toRadians(lat2-lat1);
	var dLon = toRadians(lon2-lon1);
	
	lat1 = toRadians(lat1);
	lat2 = toRadians(lat2);
	
	var y = Math.sin(dLon) * Math.cos(lat2);
	var x = Math.cos(lat1) * Math.sin(lat2) - Math.sin(lat1) * Math.cos(lat2) * Math.cos(dLon);
		
	var a = Math.sin(dLat/2) * Math.sin(dLat/2) + Math.cos(lat1) * Math.cos(lat2) * Math.sin(dLon/2) * Math.sin(dLon/2);
	var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
	
	//distance = convert_units(c*3959);
	// numeric (meters) for now since actual data doesn't appear to reach kms
	distance = (c*3959) * 1.60934 * 1000;
	
	console.log("Start Lat: " + lat1);
	console.log("Start Lon: " + lon1);
	console.log("End Lat: " + lat2);
	console.log("End Lon: " + lon2);
	console.log("Distance: " + distance);

	return distance;
}

function getBearing(myLatitude, myLongitude, pkmnLatitude, pkmnLongitude) {
	var bearing;

	var lat1 = myLatitude, lon1 = myLongitude;
	var lat2 = pkmnLatitude, lon2 = pkmnLongitude;
	
	var dLat = toRadians(lat2-lat1);
	var dLon = toRadians(lon2-lon1);
	
	lat1 = toRadians(lat1);
	lat2 = toRadians(lat2);
	
	var y = Math.sin(dLon) * Math.cos(lat2);
	var x = Math.cos(lat1) * Math.sin(lat2) - Math.sin(lat1) * Math.cos(lat2) * Math.cos(dLon);
	
	bearing = toDegrees(Math.atan2(y,x));
	if(bearing < 0) bearing = 360-Math.abs(bearing);
	
	console.log("Start Lat: " + lat1);
	console.log("Start Lon: " + lon1);
	console.log("End Lat: " + lat2);
	console.log("End Lon: " + lon2);
	console.log("Bearing: " + bearing);

	return bearing;
}

// TODO: move to own .js or maybe @mathew already has a lib?

function toRadians(degrees) {
	return degrees * Math.PI / 180;
}

function toDegrees(radians) {
	return radians * 180 / Math.PI;
}

function convert_units(old_distance) {
	var new_distance;
	
	old_distance *= 1.60934; //Convert miles to km
	
	if(old_distance < 0.1){
		new_distance = old_distance * 1000.0;
		
		if(new_distance < accuracy){
			new_distance = "< " + (accuracy/10).toFixed(0)*10 + " m";
		}
		else{
			new_distance += "";
			new_distance = new_distance.substring(0, new_distance.indexOf('.') + 2);
			new_distance += " m";
		}
	}
	else{
		new_distance = old_distance + "";
		new_distance = new_distance.substring(0, new_distance.indexOf('.') + 3);
		new_distance += " km";
	}
	//}
	
	if(new_distance.charAt(0) === '.') new_distance = "0" + new_distance;
	
	return new_distance;
}