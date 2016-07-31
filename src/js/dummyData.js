module.exports = {
  createDummyData: function(numberOfPokemon, centerLat, centerLong) {

    var response = new Object();
    response.status = "success";
    var pokemonList = [];

    var idCounter = 0;

    for(var i = 0; i < numberOfPokemon; i++) {
      var pokemon = new Object();
      pokemon.id = idCounter++ + "";
      pokemon.expiration_time = (Math.floor((new Date).getTime()/1000)) + (Math.floor(Math.random() * 600));
      pokemon.longitude = centerLong + ((Math.random() - 0.5) / 100);
      pokemon.latitiude = centerLat + ((Math.random() - 0.5) / 100);
      pokemon.uid = "HAHA F U THESE AREN'T REAL"
      pokemon.is_alive = true;
      pokemon.pokemonId = Math.floor((Math.random() / 100) * (150 - 1 + 1)) + 1;
      pokemon.data = "[]";
      pokemonList.push(pokemon);
    }

    response.pokemon = pokemonList;
    console.log(JSON.stringify(response));
    return response;
  }
};
