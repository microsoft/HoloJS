function SurfaceReconstructionExample(scene, renderer) {
    function SpatialMappingMeshes(scene) {
        var self = this;
        this.meshObjects = [];
        this.scene = scene;

        function hideMeshData() {
            for (var meshIndex in self.meshObjects) {
                self.scene.remove(self.meshObjects[meshIndex].Mesh);
            }
        }

        function showMeshData() {
            for (var meshIndex in self.meshObjects) {
                self.scene.add(self.meshObjects[meshIndex].Mesh);
            }
        }

        function idEquals(id1, id2) {
            if (id1.length !== id2.length) {
                return false;
            }

            for (var i = 0; i < id1.length; i++) {
                if (id1[i] !== id2[i]) {
                    return false;
                }
            }

            return true;
        }

        this.clearMeshData = function (id) {
            if (id !== undefined) {
                for (var meshIndex in self.meshObjects) {
                    if (idEquals(self.meshObjects[meshIndex].id, id)) {
                        self.scene.remove(self.meshObjects[meshIndex].mesh);
                        self.meshObjects[meshIndex].mesh.geometry.dispose();
                        self.meshObjects[meshIndex].mesh.material.dispose();
                        self.meshObjects.splice(meshIndex, 1);
                        break;
                    }
                }
            }
        };

        this.setMeshData = function (surfaceData) {
            self.clearMeshData(surfaceData.id);
            //var newMeshObject = createMeshObject(surfaceData);
            var newMeshObject = createMeshObjectBuffered(surfaceData);

            self.scene.add(newMeshObject.mesh);
            self.meshObjects.push(newMeshObject);
        };

        // This method is faster to render the mesh, but does not allow easy
        // manipulation of the surface; the buffers are passed through directly to the
        // GPU
        function createMeshObjectBuffered(surface) {
            var geometry = new THREE.BufferGeometry();

            // Make copies of the incoming buffers; they get recycled after this returns
            var indices = new Uint16Array(surface.indices);
            var vertices = new Float32Array(surface.vertices);
            var normals = new Uint8Array(surface.normals);

            geometry.setIndex(new THREE.BufferAttribute(indices, 1));
            geometry.addAttribute('position', new THREE.BufferAttribute(vertices, 3));
            geometry.addAttribute('normal', new THREE.BufferAttribute(normals, 3, true));

            var vertexTransform = new THREE.Matrix4();
            vertexTransform.fromArray(surface.originToSurfaceTransform);
            geometry.applyMatrix(vertexTransform);

            return { id: surface.id, mesh: new THREE.Mesh(geometry, new THREE.MeshPhongMaterial({ color: 0xFFFFFF })) };
        }

        // This method is slower than the one above, but allows for mesh manipulation
        function createMeshObject(surface) {
            var geometry = new THREE.Geometry();

            var vertexTransform = new THREE.Matrix4();
            vertexTransform.fromArray(surface.originToSurfaceTransform);

            var normalTransform = new THREE.Matrix4();
            normalTransform.fromArray(surface.normalTransform);

            var vertexIndex = 0;
            // Iterate over vertices, normals and indices and add them to the new geometry object
            for (vertexIndex = 0; vertexIndex <= surface.vertices.length - 3; vertexIndex += 3) {
                geometry.vertices.push(new THREE.Vector3(
                    surface.vertices[vertexIndex],
                    surface.vertices[vertexIndex + 1],
                    surface.vertices[vertexIndex + 2]
                ));
            }

            geometry.applyMatrix(vertexTransform);

            for (var indiceIndex = 0; indiceIndex <= surface.indices.length - 3; indiceIndex += 3) {
                var vertexNormals = [];
                for (var normalIndex = 0; normalIndex < 3; normalIndex++) {
                    vertexNormals[normalIndex] = new THREE.Vector3(
                        surface.normals[surface.indices[indiceIndex + normalIndex]],
                        surface.normals[surface.indices[indiceIndex + normalIndex] + 1],
                        surface.normals[surface.indices[indiceIndex + normalIndex] + 2]
                    );
                    vertexNormals[normalIndex].applyMatrix4(normalTransform);
                }
                geometry.faces.push(
                    //Data passed back from surface reconstruction uses front CW winding order
                    new THREE.Face3(
                        surface.indices[indiceIndex],
                        surface.indices[indiceIndex + 1],
                        surface.indices[indiceIndex + 2],
                        vertexNormals));
            }

            return { id: surface.id, mesh: new THREE.Mesh(geometry, new THREE.MeshPhongMaterial({ color: 0xFFFFFF })) };
        }
    }

    var self = this;
    self.meshManager = new SpatialMappingMeshes(scene);
    function onSurfaceAvailable(surfaceData) {
        self.meshManager.setMeshData(surfaceData);
    }

    function onSpatialSourcePress(spatialInputEvent) {
        window.spatialMappingOptions = { scanExtentMeters: { x: 5, y: 5, z: 3 }, trianglesPerCubicMeter: 100 };
        window.addEventListener("spatialmapping", onSurfaceAvailable);
    }

    // Listen to spatial input events (hands)
    // On press, spatial mapping data is requested and the visible meshes are updated
    canvas.addEventListener("sourcepress", onSpatialSourcePress);

    let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
    scene.add(ambientLight);
}