/* global itowns, document, renderer, Promise */
// # Simple Globe viewer
var exports = {};
// HOLO

//document.body.appendChild(vDiv);
//console.log(itowns);
//if( typeof itowns !== 'undefined')
var THREE = itowns.THREE;

// Integrate directly THREE Patch and THREE.HolographicCamera to use in itowns
// rename transpose shader function to avoid conflict with built-in GLES 3.0 function
for (var chunk in THREE.ShaderChunk) THREE.ShaderChunk[chunk] = THREE.ShaderChunk[chunk].replace('transpose', 'transpose2');

THREE.HolographicCamera = class HolographicCamera extends THREE.Camera {

    constructor () {
        super();

        this.type = 'HolographicCamera';
        this.isHolographicCamera = true;

        this._holographicViewMatrix = new THREE.Matrix4();
        this._holographicTransformMatrix = new THREE.Matrix4();
        this._holographicProjectionMatrix = new THREE.Matrix4();
    }
	

    update () {
        if (typeof holographic === 'undefined') return;
        let params = holographic.getHolographicCameraParameters();

        // view matrix
        this._holographicViewMatrix.fromArray(params.mid.viewMatrix); // store
        this._holographicTransformMatrix.getInverse(this._holographicViewMatrix); // invert
        this._holographicTransformMatrix.decompose(this.position, this.quaternion, this.scale); // decompose

        // projection matrix
        this._holographicProjectionMatrix.fromArray(params.mid.projectionMatrix); // store
        this.projectionMatrix.copy(this._holographicProjectionMatrix); // copy
        // TODO: Extend PerspectiveCamera and decompose fov, aspect, near, far values
    }
};

let isHoloJs = (typeof holographic !== 'undefined');

let canvas = document.createElement(isHoloJs ? 'exp-holo-canvas' : 'canvas');

if (!isHoloJs) {
    document.body.appendChild(canvas);
    document.body.style.margin = document.body.style.padding = 0;
    canvas.style.width = canvas.style.height = "100%";
}

let renderer = new THREE.WebGLRenderer({ canvas: canvas, antialias: true});//, antialias: true, logarithmicDepthBuffer: true});
renderer.setSize(window.innerWidth, window.innerHeight);
let scene = new THREE.Scene();

// Define initial camera position
//var positionOnGlobe = { longitude: 5.997, latitude: 44.98, altitude: 40000000 };

// `viewerDiv` will contain iTowns' rendering area (`<canvas>`)
//var viewerDiv2 = document.getElementById('canvasID');
//console.log(viewerDiv2);

// Instanciate iTowns GlobeView*
//var globeView = new itowns.GlobeView(canvas, positionOnGlobe, { renderer: renderer });//, { renderer: renderer });

let camera = (isHoloJs && holographic.renderMode > 0) ? new THREE.HolographicCamera() :
	new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.01, 1000);
	//globeView.camera.camera3D;

//scene.add(camera);

let clock = new THREE.Clock();
var promises = [];
var orthoLayer;
var sphereRotating;
var geometry;
var particles;

//Debug function
function addMeshToScene(sc) {
	
	var geometry2 = new THREE.SphereGeometry( 0.5, 16, 16);
	var material = new THREE.MeshBasicMaterial({side: THREE.DoubleSide, color: 0x00FF00, wireframe: true});
	var sphere = new THREE.Mesh(geometry2, material);
	sphere.position.copy(new THREE.Vector3(0, 0, - 2.5));
	sphere.updateMatrixWorld();
	
	
	var sphere2 = sphere.clone();
	sphere2.material.color = new THREE.Color(0xFF0000);
	sphere2.position.copy(new THREE.Vector3(0, 1, -2.5));
	sphere2.updateMatrixWorld();
	
	var sphere3 = sphere.clone();
	sphere3.position.copy(new THREE.Vector3(0, 1, 2.5));
	sphere3.updateMatrixWorld();
	
	var sphere4 = new THREE.Mesh(new THREE.SphereGeometry( 3.2, 16, 16), new THREE.MeshBasicMaterial({side: THREE.DoubleSide, color: 0x00FF00, wireframe: false}));
	sphere4.position.copy(new THREE.Vector3(2, 0, -25));
	sphere4.updateMatrixWorld();
	
	sc.add(sphere);
	sc.add(sphere2);
	sc.add(sphere3);
	sc.add(sphere4);
	
	sphereRotating = new THREE.Mesh(new THREE.SphereGeometry( 0.1, 16, 16), new THREE.MeshBasicMaterial({side: THREE.DoubleSide, color: 0xF0FFAF, wireframe: true}));
	sphereRotating.position.copy(new THREE.Vector3(0, 0, -1.5));
	sphereRotating.updateMatrixWorld();
	sc.add(sphereRotating);
	
	geometry = new THREE.Geometry();
	for ( i = 0; i < 5000; i ++ ) {
					var vertex = new THREE.Vector3();
					vertex.x = Math.random() * 10 - 5;
					vertex.y = Math.random() * 10 - 5;
					vertex.z = Math.random() * 10 - 5;
					geometry.vertices.push( vertex );
	}
	particles = new THREE.Points( geometry, new THREE.PointsMaterial( { size: 0.04, color: 0xFFFFFF } ) );
	
	sc.add(particles);
}

function addLayerCb(layer) {
    return globeView.addLayer(layer);
}

/*
promises.push(itowns.Fetcher.json('Ortho.json').then(addLayerCb).then(function _(l) { orthoLayer = l; }));
itowns.Fetcher.json('IGN_MNT.json').then(addLayerCb);
*/

/*
function renderInStereo(){

   // effect.render( globeView.scene, globeView.camera.camera3D);
   // controls.update();
   renderer.render(globeView.scene, camera);//globeView.camera.camera3D);
 //  renderer.setSize(window.innerWidth, window.innerHeight);
  // globeView.notifyChange(true);
}
*/

function update (delta, elapsed) {
    window.requestAnimationFrame(() => update(clock.getDelta(), clock.getElapsedTime()));
	sphereRotating.rotation.y += 0.001;
    if (camera.update) camera.update();
	particles.rotation.y+=0.001;
    renderer.render(scene, camera);
}


function start () {
    update(clock.getDelta(), clock.getElapsedTime());
}

addMeshToScene(scene);

scene.add(camera);
start();



//Promise.all(promises).then(function _() { renderer.setSize(window.innerWidth, window.innerHeight); 
//									      /*globeView.render = renderDebug;*/ addMeshToScene(sceneDebug); addMeshToScene(globeView.scene); start(); });

/*
exports.view = globeView;
exports.initialPosition = positionOnGlobe;
*/
 