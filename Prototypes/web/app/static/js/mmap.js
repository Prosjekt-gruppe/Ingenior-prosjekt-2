// class internal mazemap functionality


export class MazeMap {
    
    
    static async callPOI(poiID) {        
        try {
            const poi = await Mazemap.Data.getPoi(poiID);
            console.log("got poi:", poi)
            return {
                poiID: poi.geometry.poiID,
                names: poi.geometry.names,
                floorName: poi.geometry.floorName,
                buildingName: poi.geometry.buildingName
            };
        } catch(error) {
            console.error(`No POI found, error: ${error}`);
            return null;
        }
    }

}