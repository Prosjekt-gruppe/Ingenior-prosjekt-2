// class internal mazemap functionality


export class MazeMap {
    
    
    static async callPOI(poiID) {        
        try {
            const poi = Mazemap.Data.getPoi(303578);
            return {
                poiID: poi.geometry.poiID,
                names: poi.geometry.names,
                floorName: poi.geometry.floorName,
                buildingName: poi.geometry.buildingName
            };
        } catch(error) {
            console.error("No POI found");
            return null;
        }
    }

}