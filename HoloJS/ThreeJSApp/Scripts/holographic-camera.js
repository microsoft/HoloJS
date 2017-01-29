/**
 * Camera for rendering in HoloJS framework (https://github.com/Microsoft/HoloJS)
 * @author sjando <stuart.anderson@data61.csiro.au>
 */
THREE.HolographicCamera = class HolographicCamera extends THREE.Camera {

    constructor () {
        super();

        this.type = 'HolographicCamera';
        this.isHolographicCamera = true;

        this.left = new THREE.Camera();
        this.right = new THREE.Camera();

        // [0 = left, 1 = mid, 2 = right]
        this._cameraMap = [this.left, this, this.right];
        this._holographicViewMatrix = [new THREE.Matrix4(), new THREE.Matrix4(), new THREE.Matrix4()];
        this._holographicTransformMatrix = [new THREE.Matrix4(), new THREE.Matrix4(), new THREE.Matrix4()];
        this._holographicProjectionMatrix = [new THREE.Matrix4(), new THREE.Matrix4(), new THREE.Matrix4()];
        this._flipMatrix = new THREE.Matrix4().makeScale(-1, 1, 1);
    }

    update () {

        if (!window.experimentalHolographic) return;

        // retrieve latest camera parameters
        let params = window.getHolographicCameraParameters();
        this._holographicViewMatrix[0].fromArray(params.left.viewMatrix);
        this._holographicViewMatrix[1].fromArray(params.mid.viewMatrix);
        this._holographicViewMatrix[2].fromArray(params.right.viewMatrix);
        this._holographicProjectionMatrix[0].fromArray(params.left.projectionMatrix);
        this._holographicProjectionMatrix[1].fromArray(params.mid.projectionMatrix);
        this._holographicProjectionMatrix[2].fromArray(params.right.projectionMatrix);

        // apply view matrices
        this._holographicViewMatrix.forEach((viewMatrix, i) => {
            viewMatrix.multiply(this._flipMatrix); // flip (TODO: why is this required here but not in original sample app?)
            this._holographicTransformMatrix[i].getInverse(viewMatrix); // invert
            this._holographicTransformMatrix[i].decompose(this._cameraMap[i].position, this._cameraMap[i].quaternion, this._cameraMap[i].scale); // decompose
        });

        // apply projection matrices
        this._holographicProjectionMatrix.forEach((projectionMatrix, i) => {
            this._cameraMap[i].projectionMatrix.copy(projectionMatrix); // store
        });

        // update left/right cameras manually as they are not part of scene graph
        this.left.updateMatrixWorld();
        this.right.updateMatrixWorld();
        this.left.matrixWorldInverse.copy(this._holographicViewMatrix[0]);
        this.right.matrixWorldInverse.copy(this._holographicViewMatrix[2]);
    }
}