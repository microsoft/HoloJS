THREE.HolographicCamera = class HolographicCamera extends THREE.Camera {

    constructor () {
        super();

        this.type = 'HolographicCamera';
        this.isHolographicCamera = true;

        this._holographicViewMatrix = new THREE.Matrix4();
        this._holographicTransformMatrix = new THREE.Matrix4();
        this._holographicProjectionMatrix = new THREE.Matrix4();
        this._flipMatrix = new THREE.Matrix4().makeScale(-1, 1, 1);
        this._identityProjectionMatrix = new THREE.Matrix4();

        this._prerender = new THREE.Mesh(new THREE.BufferGeometry(), new THREE.MeshBasicMaterial());
        this._prerender.frustumCulled = false;
        this._prerender.renderOrder = Number.NEGATIVE_INFINITY;

        this._postrender = new THREE.Mesh(new THREE.BufferGeometry(), new THREE.MeshBasicMaterial());
        this._postrender.frustumCulled = false;
        this._postrender.renderOrder = Number.POSITIVE_INFINITY;

        this._prerender.onBeforeRender = (renderer, scene, camera, geometry, material, group) => {
            camera.projectionMatrix.copy(this._identityProjectionMatrix);
        };

        this._postrender.onAfterRender = (renderer, scene, camera, geometry, material, group) => {
            camera.projectionMatrix.copy(this._holographicProjectionMatrix);
        };

        this.add(this._prerender);
        this.add(this._postrender);
    }

    update () {
        if (!window.experimentalHolographic) return;

        let params = window.getHolographicCameraParameters();

        // view matrix
        this._holographicViewMatrix.fromArray(params.mid.viewMatrix);
        this._holographicViewMatrix.multiply(this._flipMatrix); // flip (TODO: Why is this required?)
        this._holographicTransformMatrix.getInverse(this._holographicViewMatrix); // invert
        this._holographicTransformMatrix.decompose(this.position, this.quaternion, this.scale); // decompose

        // projection matrix (used for culling only - do not use to render)
        this._holographicProjectionMatrix.fromArray(params.mid.projectionMatrix);
        this.projectionMatrix.copy(this._holographicProjectionMatrix);
    }
}