//"use strict";
var MessageQueue = require("./MessageQueue");

const MAX_GPS_ANGLE = 8388608;
const TRIG_MAX_ANGLE = 65536;

// How often to check website for updated pokemon (in milliseconds)
const UPDATE_FREQUENCY = 60000;


// Mock data for testing:
var use_mock_data = false;
var date = new Date();
var start_time = (date.getTime()) / 1000;  // Used for mock_data (/100 cause We need seconds, not milliseconds!)


var myLatitude = 0;
var myLongitude = 0;
var gpsErrorReported = false;


// ------------------------------------------------------------------------------------------------------------------------ //
//  Helper Functions
// ------------------------------------------------------------------------------------------------------------------------ //
var XHR_DOWNLOAD_TIMEOUT = 20000;

// Note: Add to log multiple things, e.g.: success + error to log both successes and errors
var XHR_LOG_NONE     = 0,   // No Logging
    XHR_LOG_ERRORS   = 1,   // Log errors
    XHR_LOG_SUCCESS  = 2,   // Log successes
    XHR_LOG_MESSAGES = 4,   // Log messages
    XHR_LOG_VERBOSE  = 255; // Log everything


var xhrRequest = function (url, responseType, get_or_post, params, header, xhr_log_level, success, error) {
  if(xhr_log_level & XHR_LOG_MESSAGES) console.log('[XHR] Requesting URL: "' + url + '"');
  
  var request = new XMLHttpRequest();
  
 request.xhrTimeout = setTimeout(function() {
    if(xhr_log_level & XHR_LOG_ERRORS) console.log("[XHR] Timeout Getting URL: " + url);
    request.onload = null;  // Stopping a "fail then success" scenario
    error("[XHR] Timeout Getting URL: " + url);
  }, XHR_DOWNLOAD_TIMEOUT);
  
  request.onload = function() {
    // got response, no more need for a timeout, so clear it
    clearTimeout(request.xhrTimeout); // jshint ignore:line
    
    if (this.readyState == 4 && this.status == 200) {
      if(!responseType || responseType==="" || responseType.toLowerCase()==="text") {
        if(xhr_log_level & XHR_LOG_SUCCESS) console.log("[XHR] Success: " + this.responseText);
        success(this.responseText);
      } else {
        if(xhr_log_level & XHR_LOG_SUCCESS) console.log("[XHR] Success: [" + responseType + " data]");
        success(this.response);
      }
    } else {
      if(xhr_log_level & XHR_LOG_ERRORS) console.log("[XHR] Error: " + this.responseText);
      error(this.responseText);
    }
  };
  
  request.onerror = function() {
    if(xhr_log_level & XHR_LOG_ERRORS) console.log("[XHR] Error: Unknown failure");
    error("[XHR] Error: Unknown failure");
  };

  var paramsString = "";
  if (params !== null) {
    for (var i = 0; i < params.length; i++) {
      paramsString += params[i];
      if (i < params.length - 1) {
        paramsString += "&";
      }
    }
  }

  if (get_or_post.toUpperCase() == 'GET' && paramsString !== "") {
    url += "?" + paramsString;
  }

  request.open(get_or_post.toUpperCase(), url, true);
  
  if (responseType)
    request.responseType = responseType;
  
  if (header !== null) {
    if(xhr_log_level & XHR_LOG_MESSAGES) console.log("[XHR] Header Found: "+ header[0] + " : "+ header[1]);
    request.setRequestHeader(header[0], header[1]);
  }

  if (get_or_post.toUpperCase() == 'POST') {
    request.send(paramsString);
  } else {
    request.send();
  }
};




// ----------------------------------------------------------------------------------------------------------------------------------
//  PokeVision Functions
// ------------------------------
var already_requested_pokemon = false;
//function get_pokemon(latitude, longitude) {
function get_pokemon() { 
  // quick lame hack: myLatitude, myLongitude may not yet be set - just skip for now...
  if (isNaN(myLatitude) || isNaN(myLongitude) || (myLongitude === 0 && myLatitude === 0)) {
    console.log("GPS coords not set - skipping pokevision request.");
    return;
  }

  // For constant GPS calls, only check for new pokemon the first time
  if (already_requested_pokemon) return;
  already_requested_pokemon = true;

  // Ok, we're gonna request.  But first, let's set up a timer to update every minute
  setTimeout(function() {
    already_requested_pokemon = false;
    get_pokemon();
  }, UPDATE_FREQUENCY);


  // live PokeVision data
  var scanUrl = 'https://pokevision.com/map/scan/' + myLatitude + '/' + myLongitude;
  var dataUrl = 'https://pokevision.com/map/data/' + myLatitude + '/' + myLongitude;

  if(use_mock_data) {
    // static (stable!) example of PokeVision data
    scanUrl = 'https://mathewreiss.github.io/PoGO/data.json';
    dataUrl = 'https://mathewreiss.github.io/PoGO/data.json';
  }

  // TODO: is this OK?
  xhrRequest(scanUrl, "text", "GET", null, null, XHR_LOG_ERRORS, scan_url_success, download_error);
  //xhrRequest(scanUrl, 'GET', scan_url_success);

  function scan_url_success(scanResponseText) {
    if (scanResponseText.indexOf("maintenance") > -1) {
      console.log("Servers are down for maintenance");
      // TODO: something better vs. continual pop-ups!
      MessageQueue.sendAppMessage({"DisplayMessage": "Servers are down for maintenance"});
    }

    //console.log(scanResponseText); // JSON.stringify() not necessary!

    // TODO: check scanResponseText success (although...does throttling error matter
    // since we can still view pokes from last scan...?

    xhrRequest(dataUrl, "text", "GET", null, null, XHR_LOG_ERRORS, data_url_success, download_error);
  }

}


function download_error(e) {
  console.log("Failed to access servers.  Error: " + e);
  MessageQueue.sendAppMessage({"DisplayMessage":"Failed to access servers"});
}



var allNearbyPokemon = [];
var already_downloaded_pokemon = [];

function data_url_success(dataResponseText) {
  var json = JSON.parse(dataResponseText);
  //console.log(dataResponseText); // JSON.stringify() not necessary!

  // TODO: status check!
  console.log('status is "' + json.status + '"');
  var count = json.pokemon.length;
  
  // TODO: much better error checking???
  if (count > 0) {
    for (var i = 0; i < count; i++) {
      // TODO: should still actually verify vs. using blindly!
      console.log('pokemon[' + i + '].pokemonId is "' + json.pokemon[i].pokemonId + '"');
      // PokeVision is string for some reason
      var pokemonId = Number(json.pokemon[i].pokemonId);
      var pokemonExpirationTime = json.pokemon[i].expiration_time;
      var pokemonLatitude = Number(json.pokemon[i].latitude);
      var pokemonLatitudeInteger = Math.round((pokemonLatitude / 360) * MAX_GPS_ANGLE);
      var pokemonLongitude = Number(json.pokemon[i].longitude);
      var pokemonLongitudeInteger = Math.round((pokemonLongitude / 360) * MAX_GPS_ANGLE);
      var pokemonDistance = Math.round(getDistance(myLatitude, myLongitude, pokemonLatitude, pokemonLongitude));
      
      if(use_mock_data) pokemonExpirationTime = (pokemonExpirationTime - 1469238033) + start_time;
      
      console.log('pokemonId is "' + pokemonId + '"');
      console.log('pokemonExpirationTime is "' + pokemonExpirationTime + '"');
      console.log('pokemonLatitude is "' + pokemonLatitude + '" = "' + pokemonLatitudeInteger + '"');
      console.log('pokemonLongitude is "' + pokemonLongitude + '" = "' + pokemonLongitudeInteger + '"');
      console.log("pokemonDistance = " + pokemonDistance);

      var pokemonUID = json.pokemon[i].uid;

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
        "pokemonLatitudeInteger": pokemonLatitudeInteger,
        "pokemonLongitudeInteger": pokemonLongitudeInteger,
        "pokemonDistance": pokemonDistance,
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
    for(var n=0; n<allNearbyPokemon.length-1; n++ ) {
      if (
        allNearbyPokemon[n+1] !== undefined &&
        allNearbyPokemon[n].pokemonId == allNearbyPokemon[n+1].pokemonId &&
        allNearbyPokemon[n].pokemonLatitude == allNearbyPokemon[n+1].pokemonLatitude &&
        allNearbyPokemon[n].pokemonLongitude == allNearbyPokemon[n+1].pokemonLongitude
      ) {
        console.log("duplicates: removed pokemon at index " + n);
        delete allNearbyPokemon[n];
      }
    }
    allNearbyPokemon = allNearbyPokemon.filter( function( el ){ return (typeof el !== "undefined"); } );
    

    //get rid of duplicates that have already been downloaded
    if(already_downloaded_pokemon.length > 0 && allNearbyPokemon.length > 0) {
      for(n = 0; n<allNearbyPokemon.length;) {
        var increment_n = true;
        for(var m=0; m<already_downloaded_pokemon.length; m++ ) {
          if (
            allNearbyPokemon[n].pokemonId == already_downloaded_pokemon[m].pokemonId &&
            allNearbyPokemon[n].pokemonLatitude == already_downloaded_pokemon[m].pokemonLatitude &&
            allNearbyPokemon[n].pokemonLongitude == already_downloaded_pokemon[m].pokemonLongitude
          ) {
            console.log("already have: removed pokemon at index " + n);
            increment_n = false;  // don't increment to next item in the list as the list moved for us (by deleting current list item)
            m = already_downloaded_pokemon.length;  // found the duplicate, jumping to the end
            delete allNearbyPokemon[n];
          }
        }
        if(increment_n) {
          console.log("incrementing n: " + n);
          n++;
        } else {
          console.log("filtering out deleted pokemon. n: " + n);
          // Filter out deleted pokemon
          allNearbyPokemon = allNearbyPokemon.filter( function( el ){ return (typeof el !== "undefined"); } );
        }
      }
    }
    
    // Add newely downloaded (if any) pokemon to ever-growing complete list
    for(i = 0; i<allNearbyPokemon.length; i++) {
      var pokemonData = {
        "pokemonId": allNearbyPokemon[i].pokemonId,
        "pokemonLatitude": allNearbyPokemon[i].pokemonLatitude,
        "pokemonLongitude": allNearbyPokemon[i].pokemonLongitude
      };
      already_downloaded_pokemon.push(pokemonData);
    }


    //get rid of expired pokemon
    if(allNearbyPokemon.length > 0) {
      var date = new Date();
      var now = (date.getTime()) / 1000;  // We need seconds, not milliseconds!
      //if(use_mock_data) now = 1469237613;  // Mock time, as mock data is old.
      for(n = 0; n<allNearbyPokemon.length; n++) {
        if (allNearbyPokemon[n].pokemonExpirationTime <= now) {
          console.log("expired: removed pokemon at index " + n + " Expired: " + allNearbyPokemon[n].pokemonExpirationTime + " and time is now: " + now);
          delete allNearbyPokemon[n];
        }
      }
      allNearbyPokemon = allNearbyPokemon.filter( function( el ){ return (typeof el !== "undefined"); } );
    }

    count = allNearbyPokemon.length;
  }
  
  
  // Send Pokemon to Pebble:
  if (count === 0) {
      // no pokemon found!
    MessageQueue.sendAppMessage({
        "PokemonId"             : 0
      },
      app_message_success,
      app_message_error
    );
    //MessageQueue.sendAppMessage({"DisplayMessage": "No Pokemon are nearby"});
  } else {
    // Assemble dictionary using our keys
    // send each pokemon to pebble, one by one, in order of closest to furthest
    for (var j = 0; j < count; j++) {
      // Send to Pebble
      MessageQueue.sendAppMessage(
        {
          "PokemonId"             : allNearbyPokemon[j].pokemonId,
          "PokemonExpirationTime" : allNearbyPokemon[j].pokemonExpirationTime,
          "PokemonLatitude"       : allNearbyPokemon[j].pokemonLatitudeInteger,
          "PokemonLongitude"      : allNearbyPokemon[j].pokemonLongitudeInteger
        },
        app_message_success,
        app_message_error
      );
    }
  }
}




function app_message_success(e) {
  //console.log("AppMessage sent to Pebble successfully!");
}

function app_message_error(e) {
  console.log("Error sending Pokemon to Pebble!: ");
}







// based on @mathew's process_distance()
function getDistance(myLatitude, myLongitude, pkmnLatitude, pkmnLongitude) {
  var distance;

  var lat1 = myLatitude, lon1 = myLongitude;
  var lat2 = pkmnLatitude, lon2 = pkmnLongitude;

  var dLat = toRadians(lat2-lat1);
  var dLon = toRadians(lon2-lon1);

  lat1 = toRadians(lat1);
  lat2 = toRadians(lat2);

  //var y = Math.sin(dLon) * Math.cos(lat2);
  //var x = Math.cos(lat1) * Math.sin(lat2) - Math.sin(lat1) * Math.cos(lat2) * Math.cos(dLon);

  var a = Math.sin(dLat/2) * Math.sin(dLat/2) + Math.cos(lat1) * Math.cos(lat2) * Math.sin(dLon/2) * Math.sin(dLon/2);
  var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));

  //distance = convert_units(c*3959);
  // numeric (meters) for now since actual data doesn't appear to reach kms
  distance = (c*3959) * 1.60934 * 1000;

//   console.log("Start Lat: " + lat1);
//   console.log("Start Lon: " + lon1);
//   console.log("End Lat: " + lat2);
//   console.log("End Lon: " + lon2);
//   console.log("Distance: " + distance);

  return distance;
}

// TODO: move to own .js or maybe @mathew already has a lib?

function toRadians(degrees) {
  return degrees * Math.PI / 180;
}



// ------------------------------------------------------------------------------------------------------------------------------------------------ //
//  Pebble Functions
// ------------------------------
var ready_already_ran = false;  // <-- needed cause ready can run more than once
// Listen for when the watchface is opened
Pebble.addEventListener("ready", function(e){
  console.log('PebbleKit JS ready!');
  if (ready_already_ran) return;
  ready_already_ran = true;
  // Do all init stuff here:
  init_GPS();
  // TODO: set up timer to flag already_requested_pokemon=false every minute
});



// Listen for when an AppMessage is received
Pebble.addEventListener("appmessage", function(e){
  console.log('AppMessage received!');
  already_requested_pokemon = false;  // Need to flag this as false to allow more requests
  get_pokemon();
});



// function send_pokemon_to_pebble(type, the_lat, the_lon) {
//   var lat_int = Math.round((the_lat / 360) * MAX_GPS_ANGLE);
//   var lon_int = Math.round((the_lon / 360) * MAX_GPS_ANGLE);
//   console.log("Sending player position to pebble: (" + the_lat + ", " + the_lon + ") = (" + lat_int + ", " + lon_int + ")");
//   MessageQueue.sendAppMessage({"gps_type" : type, "gps_lat" : lat_int, "gps_lon" : lon_int},
//                               function(){console.log("Successfully sent position to pebble: (" + lat_int + ", " + lon_int + ")");},
//                               function(){console.log("Failed sending position to pebble");}
//                              );
// }

function send_gps_location() {
  var lat_int = Math.round((myLatitude / 360) * MAX_GPS_ANGLE);
  var lon_int = Math.round((myLongitude / 360) * MAX_GPS_ANGLE);
  //console.log("Sending GPS position to pebble: (" + myLatitude + ", " + myLongitude + ") = (" + lat_int + ", " + lon_int + ")");
  MessageQueue.sendAppMessage({"UserLatitude" : lat_int, "UserLongitude" : lon_int},
                              function(){
                                //console.log("Successfully sent GPS to pebble: (" + lat_int + ", " + lon_int + ")");
                              },
                              function(){console.log("Failed sending GPS to pebble");}
                             );
}


// ----------------------------------------------------------------------------------------------------------------------------------
//  GPS Functions
// ------------------------------
function init_GPS() {
  if (navigator && navigator.geolocation) {
    navigator.geolocation.watchPosition(
      function(pos){ // High Accuracy GPS Location Success
        var new_lat = pos.coords.latitude;
        var new_lon = pos.coords.longitude;
        if(use_mock_data) {new_lat = 42.276710135556; new_lon = -83.734423840051;}
        if(new_lat===myLatitude && new_lon===myLongitude) return;  // If coordinates didn't change, abort
        myLatitude = new_lat; myLongitude = new_lon;
        send_gps_location();
        get_pokemon();
        gpsErrorReported = false;
      },
      function(err) { // High Accuracy Failed, Try Low Accuracy GPS Location
        console.log('High Accuracy GPS Error (' + err.code + '): ' + err.message);
        navigator.geolocation.watchPosition(
          function(pos) { // Low Accuracy Success
            var new_lat = pos.coords.latitude;
            var new_lon = pos.coords.longitude;
            if(use_mock_data) {new_lat = 42.276710135556; new_lon = -83.734423840051;}
            if(new_lat===myLatitude && new_lon===myLongitude) return;  // If coordinates didn't change, abort
            myLatitude = new_lat; myLongitude = new_lon;
            send_gps_location();
            get_pokemon();
            gpsErrorReported = false;
          },
          function(err) { // Low Accuracy GPS Location Failed: Error!
            console.log('Low Accuracy GPS Error (' + err.code + '): ' + err.message);
            // only alert once!
            if(!gpsErrorReported) {
              MessageQueue.sendAppMessage({"DisplayMessage": "Unable to detect location: make sure GPS is on"});
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
        enableHighAccuracy: true
      }
    );
  } else {  // Unable to detect GPS API (this error shouldn't happen)
    console.log('GPS API Error!');
    // only alert once!
    if(!gpsErrorReported) {
      MessageQueue.sendAppMessage({"DisplayMessage": "Unknown GPS Error!"});
      gpsErrorReported = true;
    }
  }
}
