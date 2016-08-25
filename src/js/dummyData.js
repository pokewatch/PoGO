module.exports = {
  createDummyData: function(numberOfPokemon, centerLat, centerLong) {

    var response = new Object();
    response.status = "success";
    var pokemonList = [];

    var idCounter = 0;

    for(var i = 0; i < numberOfPokemon; i++) {
      // updated to conform to new unknown6 API
      var pokemon = new Object();
      pokemon.id = idCounter++ + "";
      // note removal of / 1000 and addition of * 1000
      pokemon.disappear_time = (Math.floor((new Date).getTime())) + (Math.floor(Math.random() * 600 * 1000));
      pokemon.longitude = centerLong + ((Math.random() - 0.5) / 100);
      pokemon.latitude = centerLat + ((Math.random() - 0.5) / 100);
      pokemon.uid = "{unused}"
      pokemon.is_alive = true;
      pokemon.pokemon_id = (Math.floor(Math.random() * (150 - 1 + 1)) + 1);//.toString();
      pokemon.data = "[]";
      pokemonList.push(pokemon);
    }

    response.pokemons = pokemonList;
    console.log(JSON.stringify(response));
    return response;
  }
};
