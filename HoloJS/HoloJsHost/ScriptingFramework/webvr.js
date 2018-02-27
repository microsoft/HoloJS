function VRDisplay() {
    this.displayName = "Synthetic VR display";
    this.isPresenting = true;

    this.getFrameData = function (frameData) {
        frameData.timestamp++;

        let params = holographic.getHolographicCameraParameters();

        let viewLeft = new THREE.Matrix4();
        viewLeft.fromArray(params.left.viewMatrix);

        let viewRight = new THREE.Matrix4();
        viewRight.fromArray(params.right.viewMatrix);

        frameData.leftProjectionMatrix = params.left.projectionMatrix;
        frameData.rightProjectionMatrix = params.right.projectionMatrix;

        frameData.leftViewMatrix = params.left.viewMatrix; 
        frameData.rightViewMatrix = params.right.viewMatrix;

        frameData.pose = new VRPose(viewLeft, viewRight);
    };

    this.getLayers = function () {
        return [new VRLayerInit()];
    };

    this.submitFrame = function () {
        
    };

    this.requestAnimationFrame = function (callback) {
        holographic.drawCallback = callback;
    };
}

function VRPose(viewLeft, viewRight) {

    let viewLeftInverse = new THREE.Matrix4();
    viewLeftInverse.getInverse(viewLeft);

    let quaternion = new THREE.Quaternion();
    let position = new THREE.Vector4();
    let scale = new THREE.Vector4();
    viewLeftInverse.decompose(position, quaternion, scale); // decompose

    this.position = new Float32Array([position.x, position.y, position.z, position.w]);
    this.orientation = new Float32Array([quaternion.x, quaternion.y, quaternion.z, quaternion.w]);
}

function VRFrameData() {

    let vrCamera = new THREE.PerspectiveCamera(90, window.innerWidth / window.innerHeight, 1, 1000);
    this.timestamp = 0;

    this.leftProjectionMatrix = new Float32Array(vrCamera.projectionMatrix.elements);
    this.rightProjectionMatrix = new Float32Array(vrCamera.projectionMatrix.elements);

    this.leftViewMatrix = new Float32Array(vrCamera.modelViewMatrix.elements);
    this.rightViewMatrix = new Float32Array(vrCamera.modelViewMatrix.elements);
}

function VRLayerInit() {
    this.leftBounds = [0.0, 0.0, 0.5, 1.0];
    this.rightBounds = [0.5, 0.0, 0.5, 1.0];
}

(function () {
    holographic.getVRDisplays = function () {
        if (holographic.renderMode === 0) {
            return Promise.resolve([]);
        } else {
            return Promise.resolve([new VRDisplay()]);
        }
    };
})();
