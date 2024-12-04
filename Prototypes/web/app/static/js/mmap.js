
export class MazeMap {
    /**
     * Oppretter en ny MazeMap-instans.
     *
     * @constructor
     */
    constructor(){}

    /**
     * Asynkron funksjon for uthenting av lokasjonsdata. 
     * 
     * **Variabler som hentes fra MazeMap:**
     * 
     * - ``poiId``
     * - ``names``
     * - ``floorName``
     * - ``buildingName``
     *
     * @async
     * @static
     * @param {string} poiID Hentes fra database. 
     * @returns {Object} Returnerer utvalgt ``point of interest``-informasjon eller ``null`` 
     *     hvis ingen poi ble funnet, eller en feil oppstod.
     *
     * @example
     * const poi = await MazeMap.callPOI("36148");
     * if (poi) {
     *     console.log(poi.names);
     * }
     * // returnerer informasjon om stedet med nummer POI-nummer 36148 
     */
    static async callPOI(poiID) {        
        try {
            const poi = await Mazemap.Data.getPoi(poiID);
            console.log("got poi:", poi)
            return {
                poiId: poi.properties.poiId,
                names: poi.properties.names,
                floorName: poi.properties.floorName,
                buildingName: poi.properties.buildingName
            };
        } catch(error) {
            console.error(`No POI found, error: ${error}`);
            return null;
        }
    }

}