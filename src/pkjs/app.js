//"use strict";

// ----------------------------------------------------------------------------------------------------------------------------
//  Requires and Globals
// ------------------------------------------------------------
var MessageQueue = require("message-queue-pebble");

// clay initialization
var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
// override defaults - no need for InboxReceivedHandler vs. just keeping it in JS
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });
var messageKeys = require('message_keys');

// Mock data for testing:
var useDataUrl = 0;
var date = new Date();
var startTime = date.getTime();

// default to static data URL until config override
var staticUrl = "https://pokewatch.github.io/PoGO/unknown6.json";
var dataUrl = staticUrl;
var myLatitude, myLongitude;
var hasBeenNotified = false;
//var pkmnLatitude, pkmnLongitude;

//var distance, bearing;
var gpsErrorReported = false;

var DummyDataCreator = require("./dummyData");

var dummyData; // = DummyDataCreator.createDummyData(5, 45, -97);
//console.log(JSON.stringify(dummyData));

var firstTimeUpdatingLocation = true;
var serverError = false;


// ----------------------------------------------------------------------------------------------------------------------------
//  External Resource Acquisition
// ------------------------------------------------------------
// XMLHttpRequest helper
var xhrRequest = function (url, type, callback) {
	var xhr = new XMLHttpRequest();

	// time out after 10 seconds
	xhr.timeout = 10 * 1000;

	xhr.onload = function () {
		// request finished
		callback(this.responseText);
	};

	xhr.ontimeout = function (e) {
		// request timed out
		MessageQueue.sendAppMessage({"Message": "Data request timed out"});
		serverError = true;
	};

	xhr.open(type, url);
	xhr.send();
};



function getPokemon() { //(latitude, longitude) {

	//uncomment this to test toasts hehe "test toast"
	//MessageQueue.sendAppMessage({"Message": "Test Toast"});

	// quick lame hack: myLatitude, myLongitude may not yet be set - just skip for now...
	if (isNaN(myLatitude) || isNaN(myLongitude)) {

		console.log("GPS coords not set - skip API call...");
		console.log("myLatitude: " + myLatitude);
		console.log("myLongitude: " + myLongitude);

	} else {

		console.log("myLatitude: " + myLatitude);
		console.log("myLongitude: " + myLongitude);

		// live PokeVision data
		//var scanUrl = 'https://pokevision.com/map/scan/' + myLatitude + '/' + myLongitude;
		//var dataUrl = 'https://pokevision.com/map/data/' + myLatitude + '/' + myLongitude;

		// static (stable!) example of PokeVision data
		//var scanUrl = 'https://mathewreiss.github.io/PoGO/data.json';
		//var dataUrl = 'https://mathewreiss.github.io/PoGO/data.json';

		// static example of new back end
		//var scanUrl = 'https://pokewatch.github.io/PoGO/unknown6.json';
		//var dataUrl = 'https://pokewatch.github.io/PoGO/unknown6.json';

		/*

		// TODO: is this OK?
		xhrRequest(scanUrl, 'GET', function(scanResponseText) {
			if (scanResponseText.indexOf("maintenance") > -1) {
				console.log("Down for maintenance");

				// TODO: something better vs. continual pop-ups!
				MessageQueue.sendAppMessage({"Message": "Servers are down for maintenance"});
				serverError = true;

			} else {
				// need to check for maintenance first, otherwise we don't even have JSON
				var scanJson = JSON.parse(scanResponseText);
				console.log(scanResponseText); // JSON.stringify() not necessary!

				// TODO: should really check JSON validity too vs. assuming?

				// exclude throttling error (throttling error doesn't really matter since
				// we can still view pokes from last scan...?)
				if ((scanJson.status === "error") && (scanJson.message !== "{scan-throttle}")) {

					// advise of "demo mode" too if we're in it:
					if (dummyData != null) {
						MessageQueue.sendAppMessage({"Message": "Server error" + 
							"\n\nPlease enjoy this DEMO using DUMMY DATA! :)"});
					} else {
						MessageQueue.sendAppMessage({"Message": "Server error"});
					}
					serverError = true;
				} else {
					serverError = false;
				}

			}
		*/

		// TODO: fix indentation here during clean-up!

			xhrRequest(dataUrl, 'GET', function(dataResponseText) {
				console.log(dataResponseText); // JSON.stringify() not necessary!

				var json;
				try {
					json = JSON.parse(dataResponseText);
				} catch(e) {
					// TODO: improve!
					json = JSON.parse('{"pokemons":[]}');
					MessageQueue.sendAppMessage({"Message": "Invalid data returned by server"});
					serverError = true;
				}

				// TODO: this is silly - really should just not do xhrRequest at all, but
				// as a quick hack to enable "demo mode" for now...
				if (useDataUrl == 0) {
					// populate if not set (once)
					if (dummyData == null) {
						dummyData = DummyDataCreator.createDummyData(5, myLatitude, myLongitude);
					}
					json = dummyData;
					MessageQueue.sendAppMessage({"Message": "No server available - reverting to DEMO mode using DUMMY data..."});
				}


				console.log(dataResponseText); // JSON.stringify() not necessary!

				// TODO: status check!
				//console.log('status is "' + json.status + '"');

				// TODO: much better error checking???

				// TODO: better schema validation
				if (json.hasOwnProperty("pokemons")) {

					if (json.pokemons.length > 0) {

						if (json.pokemons[0].hasOwnProperty("disappear_time")) {

							var allNearbyPokemon = [];

							var i;
							for (i = 0; i < json.pokemons.length; i++) {

								// TODO: should still actually verify vs. using blindly!
								console.log('pokemon[' + i + '].pokemon_id is "' + json.pokemons[i].pokemon_id + '"');
								// PokeVision is string for some reason
								var pokemonId = json.pokemons[i].pokemon_id; //Number(json.pokemons[i].pokemonId);
								console.log('pokemonId is "' + pokemonId + '"');


								var pokemonExpirationTime = json.pokemons[i].disappear_time;

								// offset static times based on app launch time - ONLY when 
								// static URL specified (and enabled!)
								// TODO: care about capitalization etc.?
								if ((useDataUrl == 1) && (dataUrl == staticUrl)) {
									pokemonExpirationTime = pokemonExpirationTime - 1470884585708  + 
										(7 * 60 * 1000) + startTime;
								}

								// new data uses JS time vs. epoch, so divide by 1000 (for now/to prevent the need for changes to .c)
								pokemonExpirationTime /= 1000;

								console.log('pokemonExpirationTime is "' + pokemonExpirationTime + '"');


								var pokemonLatitude = json.pokemons[i].latitude;
								console.log('pokemonLatitude is "' + pokemonLatitude + '"');
								var pokemonLongitude = json.pokemons[i].longitude;
								console.log('pokemonLongitude is "' + pokemonLongitude + '"');

								var pokemonDistance = getDistance(myLatitude, myLongitude, pokemonLatitude, pokemonLongitude);
								var pokemonBearing = getBearing(myLatitude, myLongitude, pokemonLatitude, pokemonLongitude);

								var pokemonUID = json.pokemons[i].uid;

								// fails on iOS!
								// per @katharine:
								// > PebbleKit JS Android is not to spec.
								//allNearbyPokemon.push({i, pokemonId, pokemonExpirationTime, pokemonDistance});

								var pokemonData = {
									"i": i,
									"pokemonId": pokemonId,
									"pokemonExpirationTime": pokemonExpirationTime,
									"pokemonLatitude": pokemonLatitude,
									"pokemonLongitude": pokemonLongitude,
									"pokemonDistance": pokemonDistance,
									"pokemonBearing": pokemonBearing,
									"pokemonUID": pokemonUID
								};
								allNearbyPokemon.push(pokemonData);

							}

							console.log("allNearbyPokemon: " + JSON.stringify(allNearbyPokemon));

							// sort by distance
							allNearbyPokemon.sort(function(a, b) {
								return a.pokemonDistance - b.pokemonDistance;
							});

							//get rid of duplicates that have the same UID
							for(var i=0; i<allNearbyPokemon.length-1; i++ ) {
							  if (allNearbyPokemon[i+1] != undefined && allNearbyPokemon[i].pokemonId == allNearbyPokemon[i+1].pokemonId && allNearbyPokemon[i].pokemonLatitude == allNearbyPokemon[i+1].pokemonLatitude  && allNearbyPokemon[i].pokemonLongitude == allNearbyPokemon[i+1].pokemonLongitude) {
							    console.log("removed pokemon at index " + i)
							    delete allNearbyPokemon[i];
							    //allNearbyPokemon.splice(i, 1);
							  }
							}

							allNearbyPokemon = allNearbyPokemon.filter( function( el ){ return (typeof el !== "undefined"); } );

							// Assemble dictionary using our keys
							var dictionary = {};

							// take closest 9 (or fewer if not available; sentinel indicated by pokemonId == 0)
							var j;
							for (j = 0; j < 9; j++) {

								if (j < allNearbyPokemon.length) {
									dictionary["Pokemon" + (j + 1) + "Id"] = allNearbyPokemon[j].pokemonId;
									dictionary["Pokemon" + (j + 1) + "ExpirationTime"] = allNearbyPokemon[j].pokemonExpirationTime;
									dictionary["Pokemon" + (j + 1) + "Distance"] = allNearbyPokemon[j].pokemonDistance;
									dictionary["Pokemon" + (j + 1) + "Bearing"] = allNearbyPokemon[j].pokemonBearing;
								} else {
									dictionary["Pokemon" + (j + 1) + "Id"] = 0;
									dictionary["Pokemon" + (j + 1) + "ExpirationTime"] = 0;
									dictionary["Pokemon" + (j + 1) + "Distance"] = 0;
									dictionary["Pokemon" + (j + 1) + "Bearing"] = 0;
									break;
								}
							}
							console.log("dictionary: " + JSON.stringify(dictionary));

							// Send to Pebble
							MessageQueue.sendAppMessage(dictionary,
								function(e) {
									console.log("AppMessage sent to Pebble successfully!");
								},
								function(e) {
									console.log("Error sending AppMessage to Pebble!");
								}
							);

						} else {
							MessageQueue.sendAppMessage({"Message": "Pokemon data format incorrect"});
							serverError = true;					
						}

					} else {
						// no pokemon found!

						if (!serverError) {
							// only send if there's not already a server error toast on the way too
							MessageQueue.sendAppMessage({"Message": "No Pokemon are nearby"});
						}
					}

				} else {
					MessageQueue.sendAppMessage({"Message": "Data format incorrect"});
					serverError = true;					
				}

			});
		//});
	}
}



// ----------------------------------------------------------------------------------------------------------------------------
//  GPS Positioning Calculations
// ------------------------------------------------------------
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



// ----------------------------------------------------------------------------------------------------------------------------
//  Phone Watch Communication
// ------------------------------------------------------------
// Listen for when the watchface is opened
Pebble.addEventListener("ready", function(e){

	console.log('PebbleKit JS ready!');

    if(localStorage.notified === "yes") hasBeenNotified = true;

    if(!hasBeenNotified){
        Pebble.showSimpleNotificationOnPebble("Message from Prof. Willow", "Thanks for downloading the Pokemon GO Radar app for Pebble! You may need to rotate your wrist a few times to calibrate the compass. And, as always, remember to stay alert! Have fun!!!");
        localStorage.notified = "yes";
    }

	// load config from localStorage (or default to dummy) if not found
	// TODO: check values etc.
	if (localStorage.useDataUrl) {
		useDataUrl = localStorage.useDataUrl;
	} else {
		useDataUrl = 0;
	}
	if (localStorage.dataUrl) {
		dataUrl = localStorage.dataUrl;
	} else {
		dataUrl = staticUrl;
	}

	getLocation();
	getPokemon();
});

// Listen for when an AppMessage is received
Pebble.addEventListener("appmessage", function(e){

	console.log('AppMessage received!');

	// TODO: no need to update location due to watchPosition(), right?

	getPokemon();
});

// ----------------------------------------------------------------------------------------------------------------------------
//  Clay Configuration Settings
// ------------------------------------------------------------
// override default Clay autoHandleEvents with handling right here in JS
Pebble.addEventListener("showConfiguration", function(e) {

	console.log('showConfiguration');

	// actually...don't need to override this one, just the other, but...
	Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener("webviewclosed", function(e) {

	console.log('webviewclosed');

	if (e && !e.response) {
		return;
	}


	var claySettings = clay.getSettings(e.response);
	console.log("messageKeys.UseDataUrl: " + claySettings[messageKeys.UseDataUrl]);
	console.log("messageKeys.DataUrl: " + claySettings[messageKeys.DataUrl]);
	// note returned values are 0/1 vs. true/false
	useDataUrl = Number(claySettings[messageKeys.UseDataUrl]);
	dataUrl = claySettings[messageKeys.DataUrl];

	// TODO: check returned values before storing?
	localStorage.useDataUrl = useDataUrl;
	localStorage.dataUrl = dataUrl;
});


// ----------------------------------------------------------------------------------------------------------------------------
//  GPS Functionality
// ------------------------------------------------------------
function getLocation(){
	if(navigator && navigator.geolocation){
		navigator.geolocation.watchPosition(
			function(pos){ //Success - High Acc
				myLatitude = pos.coords.latitude;
				myLongitude = pos.coords.longitude;
				if(firstTimeUpdatingLocation) {
					getPokemon();
					firstTimeUpdatingLocation = false;
				}
				gpsErrorReported = false;
			},
			function(pos){ //Fail - High Acc
				navigator.geolocation.watchPosition(
					function(pos){ //Success - Low Acc
						myLatitude = pos.coords.latitude;
						myLongitude = pos.coords.longitude;
						if(firstTimeUpdatingLocation) {
							getPokemon();
							firstTimeUpdatingLocation = false;
						}
						gpsErrorReported = false;
					},
					function(pos){ //Fail - Low Acc
						// only alert once!
						if(!gpsErrorReported) {
							MessageQueue.sendAppMessage({"Message": "Unable to detect location: make sure GPS is on"});
							gpsErrorReported = true;
						}
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
			// only alert once!
			if(!gpsErrorReported) {
				MessageQueue.sendAppMessage({"Message": "Unable to detect location: make sure GPS is on"});
				gpsErrorReported = true;
			}
		}
	}

