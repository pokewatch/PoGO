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
      pokemon.latitude = centerLat + ((Math.random() - 0.5) / 100);
      pokemon.uid = "{unused}"
      pokemon.is_alive = true;
      pokemon.pokemonId = (Math.floor(Math.random() * (150 - 1 + 1)) + 1).toString();
      pokemon.data = "[]";
      pokemonList.push(pokemon);
    }

    response.pokemon = pokemonList;
    console.log(JSON.stringify(response));
    return response;
  }
};
