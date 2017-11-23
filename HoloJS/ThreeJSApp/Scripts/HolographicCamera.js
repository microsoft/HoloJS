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