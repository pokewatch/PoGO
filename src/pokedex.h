/*
	This File Contains all the Pokemon Information
	Kanto Pokedex
	001      ...    151
	Bulbasaur		Mew

*/

#define NUM_ALL_POKEMON 151

static char * poke_names[] = {
" 001      \n\nBulbasaur\n\nGrass\nPoison",
" 002      \n\nIvysaur\n\nGrass\nPoison",
" 003      \n\nVenusaur\n\nGrass\nPoison",
" 004      \n\nCharmander\n\nFire\n",
" 005      \n\nCharmeleon\n\nFire\n",
" 006      \n\nCharizard\n\nFire\nFlying",
" 007      \n\nSquirtle\n\nWater\n",
" 008      \n\nWartortle\n\nWater\n",
" 009      \n\nBlastoise\n\nWater\n",
" 010      \n\nCaterpie\n\nBug\n",
" 011      \n\nMetapod\n\nBug\n",
" 012      \n\nButterfree\n\nBug\nFlying",
" 013      \n\nWeedle\n\nBug\nPoison",
" 014      \n\nKakuna\n\nBug\nPoison",
" 015      \n\nBeedrill\n\nBug\nPoison",
" 016      \n\nPidgey\n\nNormal\nFlying",
" 017      \n\nPidgeotto\n\nNormal\nFlying",
" 018      \n\nPidgeot\n\nNormal\nFlying",
" 019      \n\nRattata\n\nNormal\n",
" 020      \n\nRaticate\n\nNormal\n",
" 021      \n\nSpearow\n\nNormal\nFlying",
" 022      \n\nFearow\n\nNormal\nFlying",
" 023      \n\nEkans\n\nPoison\n",
" 024      \n\nArbok\n\nPoison\n",
" 025      \n\nPikachu\n\nElectric\n",
" 026      \n\nRaichu\n\nElectric\n",
" 027      \n\nSandshrew\n\nGround\n",
" 028      \n\nSandslash\n\nGround\n",
" 029      \n\nNidoran F\n\nPoison\n",
" 030      \n\nNidorina\n\nPoison\n",
" 031      \n\nNidoqueen\n\nPoison\nGround",
" 032      \n\nNidoran M\n\nPoison\n",
" 033      \n\nNidorino\n\nPoison\n",
" 034      \n\nNidoking\n\nPoison\nGround",
" 035      \n\nClefairy\n\nNormal\n",
" 036      \n\nClefable\n\nNormal\n",
" 037      \n\nVulpix\n\nFire\n",
" 038      \n\nNinetales\n\nFire\n",
" 039      \n\nJigglypuff\n\nNormal\n",
" 040      \n\nWigglytuff\n\nNormal\n",
" 041      \n\nZubat\n\nPoison\nFlying",
" 042      \n\nGolbat\n\nPoison\nFlying",
" 043      \n\nOddish\n\nGrass\nPoison",
" 044      \n\nGloom\n\nGrass\nPoison",
" 045      \n\nVileplume\n\nGrass\nPoison",
" 046      \n\nParas\n\nBug\nGrass",
" 047      \n\nParasect\n\nBug\nGrass",
" 048      \n\nVenonat\n\nBug\nPoison",
" 049      \n\nVenomoth\n\nBug\nPoison",
" 050      \n\nDiglett\n\nGround\n",
" 051      \n\nDugtrio\n\nGround\n",
" 052      \n\nMeowth\n\nNormal\n",
" 053      \n\nPersian\n\nNormal\n",
" 054      \n\nPsyduck\n\nWater\n",
" 055      \n\nGolduck\n\nWater\n",
" 056      \n\nMankey\n\nFighting\n",
" 057      \n\nPrimeape\n\nFighting\n",
" 058      \n\nGrowlithe\n\nFire\n",
" 059      \n\nArcanine\n\nFire\n",
" 060      \n\nPoliwag\n\nWater\n",
" 061      \n\nPoliwhirl\n\nWater\n",
" 062      \n\nPoliwrath\n\nWater\nFighting",
" 063      \n\nAbra\n\nPsychic\n",
" 064      \n\nKadabra\n\nPsychic\n",
" 065      \n\nAlakazam\n\nPsychic\n",
" 066      \n\nMachop\n\nFighting\n",
" 067      \n\nMachoke\n\nFighting\n",
" 068      \n\nMachamp\n\nFighting\n",
" 069      \n\nBellsprout\n\nGrass\nPoison",
" 070      \n\nWeepinbell\n\nGrass\nPoison",
" 071      \n\nVictreebel\n\nGrass\nPoison",
" 072      \n\nTentacool\n\nWater\nPoison",
" 073      \n\nTentacruel\n\nWater\nPoison",
" 074      \n\nGeodude\n\nRock\nGround",
" 075      \n\nGraveler\n\nRock\nGround",
" 076      \n\nGolem\n\nRock\nGround",
" 077      \n\nPonyta\n\nFire\n",
" 078      \n\nRapidash\n\nFire\n",
" 079      \n\nSlowpoke\n\nWater\nPsychic",
" 080      \n\nSlowbro\n\nWater\nPsychic",
" 081      \n\nMagnemite\n\nElectric\nSteel",
" 082      \n\nMagneton\n\nElectric\nSteel",
" 083      \n\nFarfetch'd\n\nNormal\nFlying",
" 084      \n\nDoduo\n\nNormal\nFlying",
" 085      \n\nDodrio\n\nNormal\nFlying",
" 086      \n\nSeel\n\nWater\n",
" 087      \n\nDewgong\n\nWater\nIce",
" 088      \n\nGrimer\n\nPoison\n",
" 089      \n\nMuk\n\nPoison\n",
" 090      \n\nShellder\n\nWater\n",
" 091      \n\nCloyster\n\nWater\nIce",
" 092      \n\nGastly\n\nGhost\nPoison",
" 093      \n\nHaunter\n\nGhost\nPoison",
" 094      \n\nGengar\n\nGhost\nPoison",
" 095      \n\nOnix\n\nRock\nGround",
" 096      \n\nDrowzee\n\nPsychic\n",
" 097      \n\nHypno\n\nPsychic\n",
" 098      \n\nKrabby\n\nWater\n",
" 099      \n\nKingler\n\nWater\n",
" 100      \n\nVoltorb\n\nElectric\n",
" 101      \n\nElectrode\n\nElectric\n",
" 102      \n\nExeggcute\n\nGrass\nPsychic",
" 103      \n\nExeggutor\n\nGrass\nPsychic",
" 104      \n\nCubone\n\nGround\n",
" 105      \n\nMarowak\n\nGround\n",
" 106      \n\nHitmonlee\n\nFighting\n",
" 107      \n\nHitmonchan\n\nFighting\n",
" 108      \n\nLickitung\n\nNormal\n",
" 109      \n\nKoffing\n\nPoison\n",
" 110      \n\nWeezing\n\nPoison\n",
" 111      \n\nRhyhorn\n\nGround\nRock",
" 112      \n\nRhydon\n\nGround\nRock",
" 113      \n\nChansey\n\nNormal\n",
" 114      \n\nTangela\n\nGrass\n",
" 115      \n\nKangaskhan\n\nNormal\n",
" 116      \n\nHorsea\n\nWater\n",
" 117      \n\nSeadra\n\nWater\n",
" 118      \n\nGoldeen\n\nWater\n",
" 119      \n\nSeaking\n\nWater\n",
" 120      \n\nStaryu\n\nWater\n",
" 121      \n\nStarmie\n\nWater\nPsychic",
" 122      \n\nMr. Mime\n\nPsychic\n",
" 123      \n\nScyther\n\nBug\nFlying",
" 124      \n\nJynx\n\nIce\nPsychic",
" 125      \n\nElectabuzz\n\nElectric\n",
" 126      \n\nMagmar\n\nFire\n",
" 127      \n\nPinsir\n\nBug\n",
" 128      \n\nTauros\n\nNormal\n",
" 129      \n\nMagikarp\n\nWater\n",
" 130      \n\nGyarados\n\nWater\nFlying",
" 131      \n\nLapras\n\nWater\nIce",
" 132      \n\nDitto\n\nNormal\n",
" 133      \n\nEevee\n\nNormal\n",
" 134      \n\nVaporeon\n\nWater\n",
" 135      \n\nJolteon\n\nElectric\n",
" 136      \n\nFlareon\n\nFire\n",
" 137      \n\nPorygon\n\nNormal\n",
" 138      \n\nOmanyte\n\nRock\nWater",
" 139      \n\nOmastar\n\nRock\nWater",
" 140      \n\nKabuto\n\nRock\nWater",
" 141      \n\nKabutops\n\nRock\nWater",
" 142      \n\nAerodactyl\n\nRock\nFlying",
" 143      \n\nSnorlax\n\nNormal\n",
" 144      \n\nArticuno\n\nIce\nFlying",
" 145      \n\nZapdos\n\nElectric\nFlying",
" 146      \n\nMoltres\n\nFire\nFlying",
" 147      \n\nDratini\n\nDragon\n",
" 148      \n\nDragonair\n\nDragon\n",
" 149      \n\nDragonite\n\nDragon\nFlying",
" 150      \n\nMewtwo\n\nPsychic\n",
" 151      \n\nMew\n\nPsychic\n",
" ?&0      \n\nMissingNo\n\n?????\n",
NULL};

static char * poke_info[] = {
" 001      \n\nBulbasaur\n\n0.7  m\n6.9 kg",
" 002      \n\nIvysaur\n\n1.0  m\n13.0 kg",
" 003      \n\nVenusaur\n\n2.0  m\n100.0 kg",
" 004      \n\nCharmander\n\n0.6  m\n8.5 kg",
" 005      \n\nCharmeleon\n\n1.1  m\n19.0 kg",
" 006      \n\nCharizard\n\n1.7  m\n90.5 kg",
" 007      \n\nSquirtle\n\n0.5  m\n9.0 kg",
" 008      \n\nWartortle\n\n1.0  m\n22.5 kg",
" 009      \n\nBlastoise\n\n1.6  m\n85.5 kg",
" 010      \n\nCaterpie\n\n0.3  m\n2.9 kg",
" 011      \n\nMetapod\n\n0.7  m\n9.9 kg",
" 012      \n\nButterfree\n\n1.1  m\n32.0 kg",
" 013      \n\nWeedle\n\n0.3  m\n3.2 kg",
" 014      \n\nKakuna\n\n0.6  m\n10.0 kg",
" 015      \n\nBeedrill\n\n1.0  m\n29.5 kg",
" 016      \n\nPidgey\n\n0.3  m\n1.8 kg",
" 017      \n\nPidgeotto\n\n1.1  m\n30.0 kg",
" 018      \n\nPidgeot\n\n1.5  m\n39.5 kg",
" 019      \n\nRattata\n\n0.3  m\n3.5 kg",
" 020      \n\nRaticate\n\n0.7  m\n18.5 kg",
" 021      \n\nSpearow\n\n0.3  m\n2.0 kg",
" 022      \n\nFearow\n\n1.2  m\n38.0 kg",
" 023      \n\nEkans\n\n2.0  m\n6.9 kg",
" 024      \n\nArbok\n\n3.5  m\n65.0 kg",
" 025      \n\nPikachu\n\n0.4  m\n6.0 kg",
" 026      \n\nRaichu\n\n0.8  m\n30.0 kg",
" 027      \n\nSandshrew\n\n0.6  m\n12.0 kg",
" 028      \n\nSandslash\n\n1.0  m\n29.5 kg",
" 029      \n\nNidoran F\n\n0.4  m\n7.0 kg",
" 030      \n\nNidorina\n\n0.8  m\n20.0 kg",
" 031      \n\nNidoqueen\n\n1.3  m\n60.0 kg",
" 032      \n\nNidoran M\n\n0.5  m\n9.0 kg",
" 033      \n\nNidorino\n\n0.9  m\n19.5 kg",
" 034      \n\nNidoking\n\n1.4  m\n62.0 kg",
" 035      \n\nClefairy\n\n0.6  m\n7.5 kg",
" 036      \n\nClefable\n\n1.3  m\n40.0 kg",
" 037      \n\nVulpix\n\n0.6  m\n9.9 kg",
" 038      \n\nNinetales\n\n1.1  m\n19.9 kg",
" 039      \n\nJigglypuff\n\n0.5  m\n5.5 kg",
" 040      \n\nWigglytuff\n\n1.0  m\n12.0 kg",
" 041      \n\nZubat\n\n0.8  m\n7.5 kg",
" 042      \n\nGolbat\n\n1.6  m\n55.0 kg",
" 043      \n\nOddish\n\n0.5  m\n5.4 kg",
" 044      \n\nGloom\n\n0.8  m\n8.6 kg",
" 045      \n\nVileplume\n\n1.2  m\n18.6 kg",
" 046      \n\nParas\n\n0.3  m\n5.4 kg",
" 047      \n\nParasect\n\n1.0  m\n29.5 kg",
" 048      \n\nVenonat\n\n1.0  m\n30.0 kg",
" 049      \n\nVenomoth\n\n1.5  m\n12.5 kg",
" 050      \n\nDiglett\n\n0.2  m\n0.8 kg",
" 051      \n\nDugtrio\n\n0.7  m\n33.3 kg",
" 052      \n\nMeowth\n\n0.4  m\n4.2 kg",
" 053      \n\nPersian\n\n1.0  m\n32.0 kg",
" 054      \n\nPsyduck\n\n0.8  m\n19.6 kg",
" 055      \n\nGolduck\n\n1.7  m\n76.6 kg",
" 056      \n\nMankey\n\n0.5  m\n28.0 kg",
" 057      \n\nPrimeape\n\n1.0  m\n32.0 kg",
" 058      \n\nGrowlithe\n\n0.7  m\n19.0 kg",
" 059      \n\nArcanine\n\n1.9  m\n155.0 kg",
" 060      \n\nPoliwag\n\n0.6  m\n12.4 kg",
" 061      \n\nPoliwhirl\n\n1.0  m\n20.0 kg",
" 062      \n\nPoliwrath\n\n1.3  m\n54.0 kg",
" 063      \n\nAbra\n\n0.9  m\n19.5 kg",
" 064      \n\nKadabra\n\n1.3  m\n56.5 kg",
" 065      \n\nAlakazam\n\n1.5  m\n48.0 kg",
" 066      \n\nMachop\n\n0.8  m\n19.5 kg",
" 067      \n\nMachoke\n\n1.5  m\n70.5 kg",
" 068      \n\nMachamp\n\n1.6  m\n130.0 kg",
" 069      \n\nBellsprout\n\n0.7  m\n4.0 kg",
" 070      \n\nWeepinbell\n\n1.0  m\n6.4 kg",
" 071      \n\nVictreebel\n\n1.7  m\n15.5 kg",
" 072      \n\nTentacool\n\n0.9  m\n45.5 kg",
" 073      \n\nTentacruel\n\n1.6  m\n55.0 kg",
" 074      \n\nGeodude\n\n0.4  m\n20.0 kg",
" 075      \n\nGraveler\n\n1.0  m\n105.0 kg",
" 076      \n\nGolem\n\n1.4  m\n300.0 kg",
" 077      \n\nPonyta\n\n1.0  m\n30.0 kg",
" 078      \n\nRapidash\n\n1.7  m\n95.0 kg",
" 079      \n\nSlowpoke\n\n1.2  m\n36.0 kg",
" 080      \n\nSlowbro\n\n1.6  m\n78.5 kg",
" 081      \n\nMagnemite\n\n0.3  m\n6.0 kg",
" 082      \n\nMagneton\n\n1.0  m\n60.0 kg",
" 083      \n\nFarfetch'd\n\n0.8  m\n15.0 kg",
" 084      \n\nDoduo\n\n1.4  m\n39.2 kg",
" 085      \n\nDodrio\n\n1.8  m\n85.2 kg",
" 086      \n\nSeel\n\n1.1  m\n90.0 kg",
" 087      \n\nDewgong\n\n1.7  m\n120.0 kg",
" 088      \n\nGrimer\n\n0.9  m\n30.0 kg",
" 089      \n\nMuk\n\n1.2  m\n30.0 kg",
" 090      \n\nShellder\n\n0.3  m\n4.0 kg",
" 091      \n\nCloyster\n\n1.5  m\n132.5 kg",
" 092      \n\nGastly\n\n1.3  m\n0.1 kg",
" 093      \n\nHaunter\n\n1.6  m\n0.1 kg",
" 094      \n\nGengar\n\n1.5  m\n40.5 kg",
" 095      \n\nOnix\n\n8.8  m\n210.0 kg",
" 096      \n\nDrowzee\n\n1.0  m\n32.4 kg",
" 097      \n\nHypno\n\n1.6  m\n75.6 kg",
" 098      \n\nKrabby\n\n0.4  m\n6.5 kg",
" 099      \n\nKingler\n\n1.3  m\n60.0 kg",
" 100      \n\nVoltorb\n\n0.5  m\n10.4 kg",
" 101      \n\nElectrode\n\n1.2  m\n66.6 kg",
" 102      \n\nExeggcute\n\n0.4  m\n2.5 kg",
" 103      \n\nExeggutor\n\n2.0  m\n120.0 kg",
" 104      \n\nCubone\n\n0.4  m\n6.5 kg",
" 105      \n\nMarowak\n\n1.0  m\n45.0 kg",
" 106      \n\nHitmonlee\n\n1.5  m\n49.8 kg",
" 107      \n\nHitmonchan\n\n1.4  m\n50.2 kg",
" 108      \n\nLickitung\n\n1.2  m\n65.5 kg",
" 109      \n\nKoffing\n\n0.6  m\n1.0 kg",
" 110      \n\nWeezing\n\n1.2  m\n9.5 kg",
" 111      \n\nRhyhorn\n\n1.0  m\n115.0 kg",
" 112      \n\nRhydon\n\n1.9  m\n120.0 kg",
" 113      \n\nChansey\n\n1.1  m\n34.6 kg",
" 114      \n\nTangela\n\n1.0  m\n35.0 kg",
" 115      \n\nKangaskhan\n\n2.2  m\n80.0 kg",
" 116      \n\nHorsea\n\n0.4  m\n8.0 kg",
" 117      \n\nSeadra\n\n1.2  m\n25.0 kg",
" 118      \n\nGoldeen\n\n0.6  m\n15.0 kg",
" 119      \n\nSeaking\n\n1.3  m\n39.0 kg",
" 120      \n\nStaryu\n\n0.8  m\n34.5 kg",
" 121      \n\nStarmie\n\n1.1  m\n80.0 kg",
" 122      \n\nMr.Mime\n\n1.3  m\n54.5 kg",
" 123      \n\nScyther\n\n1.5  m\n56.0 kg",
" 124      \n\nJynx\n\n1.4  m\n40.6 kg",
" 125      \n\nElectabuzz\n\n1.1  m\n30.0 kg",
" 126      \n\nMagmar\n\n1.3  m\n44.5 kg",
" 127      \n\nPinsir\n\n1.5  m\n55.0 kg",
" 128      \n\nTauros\n\n1.4  m\n88.4 kg",
" 129      \n\nMagikarp\n\n0.9  m\n10.0 kg",
" 130      \n\nGyarados\n\n6.5  m\n235.0 kg",
" 131      \n\nLapras\n\n2.5  m\n220.0 kg",
" 132      \n\nDitto\n\n0.3  m\n4.0 kg",
" 133      \n\nEevee\n\n0.3  m\n6.5 kg",
" 134      \n\nVaporeon\n\n1.0  m\n29.0 kg",
" 135      \n\nJolteon\n\n0.8  m\n24.5 kg",
" 136      \n\nFlareon\n\n0.9  m\n25.0 kg",
" 137      \n\nPorygon\n\n0.8  m\n36.5 kg",
" 138      \n\nOmanyte\n\n0.4  m\n7.5 kg",
" 139      \n\nOmastar\n\n1.0  m\n35.0 kg",
" 140      \n\nKabuto\n\n0.5  m\n11.5 kg",
" 141      \n\nKabutops\n\n1.3  m\n40.5 kg",
" 142      \n\nAerodactyl\n\n1.8  m\n59.0 kg",
" 143      \n\nSnorlax\n\n2.1  m\n460.0 kg",
" 144      \n\nArticuno\n\n1.7  m\n55.4 kg",
" 145      \n\nZapdos\n\n1.6  m\n52.6 kg",
" 146      \n\nMoltres\n\n2.0  m\n60.0 kg",
" 147      \n\nDratini\n\n1.8  m\n3.3 kg",
" 148      \n\nDragonair\n\n4.0  m\n16.5 kg",
" 149      \n\nDragonite\n\n2.2  m\n210.0 kg",
" 150      \n\nMewtwo\n\n2.0  m\n122.0 kg",
" 151      \n\nMew\n\n0.4  m\n4.0 kg",
NULL};



static uint8_t poke_images[] = { \
RESOURCE_ID_IMAGE_poke1,\
RESOURCE_ID_IMAGE_poke2,\
RESOURCE_ID_IMAGE_poke3,\
RESOURCE_ID_IMAGE_poke4,\
RESOURCE_ID_IMAGE_poke5,\
RESOURCE_ID_IMAGE_poke6,\
RESOURCE_ID_IMAGE_poke7,\
RESOURCE_ID_IMAGE_poke8,\
RESOURCE_ID_IMAGE_poke9,\
RESOURCE_ID_IMAGE_poke10,\
RESOURCE_ID_IMAGE_poke11,\
RESOURCE_ID_IMAGE_poke12,\
RESOURCE_ID_IMAGE_poke13,\
RESOURCE_ID_IMAGE_poke14,\
RESOURCE_ID_IMAGE_poke15,\
RESOURCE_ID_IMAGE_poke16,\
RESOURCE_ID_IMAGE_poke17,\
RESOURCE_ID_IMAGE_poke18,\
RESOURCE_ID_IMAGE_poke19,\
RESOURCE_ID_IMAGE_poke20,\
RESOURCE_ID_IMAGE_poke21,\
RESOURCE_ID_IMAGE_poke22,\
RESOURCE_ID_IMAGE_poke23,\
RESOURCE_ID_IMAGE_poke24,\
RESOURCE_ID_IMAGE_poke25,\
RESOURCE_ID_IMAGE_poke26,\
RESOURCE_ID_IMAGE_poke27,\
RESOURCE_ID_IMAGE_poke28,\
RESOURCE_ID_IMAGE_poke29,\
RESOURCE_ID_IMAGE_poke30,\
RESOURCE_ID_IMAGE_poke31,\
RESOURCE_ID_IMAGE_poke32,\
RESOURCE_ID_IMAGE_poke33,\
RESOURCE_ID_IMAGE_poke34,\
RESOURCE_ID_IMAGE_poke35,\
RESOURCE_ID_IMAGE_poke36,\
RESOURCE_ID_IMAGE_poke37,\
RESOURCE_ID_IMAGE_poke38,\
RESOURCE_ID_IMAGE_poke39,\
RESOURCE_ID_IMAGE_poke40,\
RESOURCE_ID_IMAGE_poke41,\
RESOURCE_ID_IMAGE_poke42,\
RESOURCE_ID_IMAGE_poke43,\
RESOURCE_ID_IMAGE_poke44,\
RESOURCE_ID_IMAGE_poke45,\
RESOURCE_ID_IMAGE_poke46,\
RESOURCE_ID_IMAGE_poke47,\
RESOURCE_ID_IMAGE_poke48,\
RESOURCE_ID_IMAGE_poke49,\
RESOURCE_ID_IMAGE_poke50,\
RESOURCE_ID_IMAGE_poke51,\
RESOURCE_ID_IMAGE_poke52,\
RESOURCE_ID_IMAGE_poke53,\
RESOURCE_ID_IMAGE_poke54,\
RESOURCE_ID_IMAGE_poke55,\
RESOURCE_ID_IMAGE_poke56,\
RESOURCE_ID_IMAGE_poke57,\
RESOURCE_ID_IMAGE_poke58,\
RESOURCE_ID_IMAGE_poke59,\
RESOURCE_ID_IMAGE_poke60,\
RESOURCE_ID_IMAGE_poke61,\
RESOURCE_ID_IMAGE_poke62,\
RESOURCE_ID_IMAGE_poke63,\
RESOURCE_ID_IMAGE_poke64,\
RESOURCE_ID_IMAGE_poke65,\
RESOURCE_ID_IMAGE_poke66,\
RESOURCE_ID_IMAGE_poke67,\
RESOURCE_ID_IMAGE_poke68,\
RESOURCE_ID_IMAGE_poke69,\
RESOURCE_ID_IMAGE_poke70,\
RESOURCE_ID_IMAGE_poke71,\
RESOURCE_ID_IMAGE_poke72,\
RESOURCE_ID_IMAGE_poke73,\
RESOURCE_ID_IMAGE_poke74,\
RESOURCE_ID_IMAGE_poke75,\
RESOURCE_ID_IMAGE_poke76,\
RESOURCE_ID_IMAGE_poke77,\
RESOURCE_ID_IMAGE_poke78,\
RESOURCE_ID_IMAGE_poke79,\
RESOURCE_ID_IMAGE_poke80,\
RESOURCE_ID_IMAGE_poke81,\
RESOURCE_ID_IMAGE_poke82,\
RESOURCE_ID_IMAGE_poke83,\
RESOURCE_ID_IMAGE_poke84,\
RESOURCE_ID_IMAGE_poke85,\
RESOURCE_ID_IMAGE_poke86,\
RESOURCE_ID_IMAGE_poke87,\
RESOURCE_ID_IMAGE_poke88,\
RESOURCE_ID_IMAGE_poke89,\
RESOURCE_ID_IMAGE_poke90,\
RESOURCE_ID_IMAGE_poke91,\
RESOURCE_ID_IMAGE_poke92,\
RESOURCE_ID_IMAGE_poke93,\
RESOURCE_ID_IMAGE_poke94,\
RESOURCE_ID_IMAGE_poke95,\
RESOURCE_ID_IMAGE_poke96,\
RESOURCE_ID_IMAGE_poke97,\
RESOURCE_ID_IMAGE_poke98,\
RESOURCE_ID_IMAGE_poke99,\
RESOURCE_ID_IMAGE_poke100,\
RESOURCE_ID_IMAGE_poke101,\
RESOURCE_ID_IMAGE_poke102,\
RESOURCE_ID_IMAGE_poke103,
RESOURCE_ID_IMAGE_poke104,
RESOURCE_ID_IMAGE_poke105,
RESOURCE_ID_IMAGE_poke106,
RESOURCE_ID_IMAGE_poke107,
RESOURCE_ID_IMAGE_poke108,
RESOURCE_ID_IMAGE_poke109,
RESOURCE_ID_IMAGE_poke110,
RESOURCE_ID_IMAGE_poke111,
RESOURCE_ID_IMAGE_poke112,
RESOURCE_ID_IMAGE_poke113,
RESOURCE_ID_IMAGE_poke114,
RESOURCE_ID_IMAGE_poke115,
RESOURCE_ID_IMAGE_poke116,
RESOURCE_ID_IMAGE_poke117,
RESOURCE_ID_IMAGE_poke118,
RESOURCE_ID_IMAGE_poke119,
RESOURCE_ID_IMAGE_poke120,
RESOURCE_ID_IMAGE_poke121,
RESOURCE_ID_IMAGE_poke122,
RESOURCE_ID_IMAGE_poke123,
RESOURCE_ID_IMAGE_poke124,
RESOURCE_ID_IMAGE_poke125,
RESOURCE_ID_IMAGE_poke126,
RESOURCE_ID_IMAGE_poke127,
RESOURCE_ID_IMAGE_poke128,
RESOURCE_ID_IMAGE_poke129,
RESOURCE_ID_IMAGE_poke130,
RESOURCE_ID_IMAGE_poke131,
RESOURCE_ID_IMAGE_poke132,
RESOURCE_ID_IMAGE_poke133,
RESOURCE_ID_IMAGE_poke134,
RESOURCE_ID_IMAGE_poke135,
RESOURCE_ID_IMAGE_poke136,
RESOURCE_ID_IMAGE_poke137,
RESOURCE_ID_IMAGE_poke138,
RESOURCE_ID_IMAGE_poke139,
RESOURCE_ID_IMAGE_poke140,
RESOURCE_ID_IMAGE_poke141,
RESOURCE_ID_IMAGE_poke142,
RESOURCE_ID_IMAGE_poke143,
RESOURCE_ID_IMAGE_poke144,
RESOURCE_ID_IMAGE_poke145,
RESOURCE_ID_IMAGE_poke146,
RESOURCE_ID_IMAGE_poke147,
RESOURCE_ID_IMAGE_poke148,
RESOURCE_ID_IMAGE_poke149,
RESOURCE_ID_IMAGE_poke150,
RESOURCE_ID_IMAGE_poke151
};
