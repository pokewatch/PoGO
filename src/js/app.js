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

function getPokemon() {

	// static (stable!) example of PokeVision data
	//var url = 'https://mathewreiss.github.io/PoGO/data.json';

	// live PokeVision data, hard-coded to Ann Arbor for now
	var url = 'https://pokevision.com/map/data/42.277556681/-83.740878574';

	xhrRequest(url, 'GET', 
		function(responseText) {
			var json = JSON.parse(responseText);
			console.log(responseText); // JSON.stringify() not necessary!

			// TODO: status check!
			console.log('status is "' + json.status + '"');

			// TODO: error checking???
			console.log('pokemon[0].pokemonId is "' + json.pokemon[0].pokemonId + '"');
			// PokeVision is string for some reason
			var pokemonId = Number(json.pokemon[0].pokemonId);
			console.log('pokemonId is "' + pokemonId + '"');

			// Assemble dictionary using our keys
			var dictionary = {
				"PokemonId": pokemonId
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

		}
	);

}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
	function(e) {
		console.log('PebbleKit JS ready!');

		getPokemon();
	}
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
	function(e) {
		console.log('AppMessage received!');

		getPokemon();
	}                     
);