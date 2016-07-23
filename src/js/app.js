
function getPokemon() {
	responseText = '{"status":"success","pokemon":[{"id":"77528950","data":"[]","expiration_time":1469238283,"pokemonId":"39","latitude":42.276610135556,"longitude":-83.732323840051,"uid":"883cae449c7:19","is_alive":true}]}';

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