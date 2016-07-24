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

						// Assemble dictionary using our keys
						var dictionary = {
							"PokemonId": pokemonId,
							"PokemonExpirationTime": pokemonExpirationTime,
							"PokemonLatitude": pokemonLatitude,
							"PokemonLongitude": pokemonLongitude
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