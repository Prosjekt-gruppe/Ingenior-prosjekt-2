// class internal mazemap functionality


export class MazeMap {
    
    
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