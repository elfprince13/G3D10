/**
  \file App.h

  The G3D 10.00 default starter app is configured for OpenGL 3.3 and
  relatively recent GPUs.
 */
#ifndef App_h
#define App_h
#include <physx/GLG3DPhysXAll.h>

class PhysXWorld : public ReferenceCountedObject {
public:
    PxFoundation*           foundation;
    PxProfileZoneManager*   profileZoneManager;
    PxPhysics*              physics;
    PxMaterial*             defaultMaterial;
    PxCooking*              cooking;
    PxScene*                scene;
    PxCpuDispatcher*        cpuDispatcher;

protected:

    PhysXWorld();

public:

    static shared_ptr<PhysXWorld> create() {
        return shared_ptr<PhysXWorld>(new PhysXWorld());
    }

    virtual ~PhysXWorld();

    PxTriangleMesh* cookTriangleMesh(const Array<Vector3>& vertices, const Array<int>& indices);

    /** Designed to mirror G3D::TriTree */
    class TriTree : public ReferenceCountedObject {
    protected:
        Array<Tri>              m_triArray;
        CPUVertexArray          m_cpuVertexArray;
        PhysXWorld*             m_world;
        PxTriangleMeshGeometry* m_geometry;

        TriTree(PhysXWorld* world) : m_world(world), m_geometry(nullptr) {}

    public:

        static shared_ptr<TriTree> create(PhysXWorld* world) {
            return shared_ptr<TriTree>(new TriTree(world));
        }

        virtual ~TriTree();

        void clear();

        void setContents(const Array<shared_ptr<Surface>>& surfaceArray, ImageStorage newStorage = COPY_TO_CPU); 

        bool intersectRay(Ray ray, Tri::Intersector& intersectCallback, float& distance, bool exitOnAnyHit = false, bool twoSided = false) const;

        shared_ptr<Surfel> intersectRay(const Ray& ray, float& distance, bool exitOnAnyHit = false, bool twoSided = false) const;
    };
};


/** Application framework. */
class App : public GApp {
protected:

    /** Called from onInit */
    void makeGUI();

public:
    
    shared_ptr<PhysXWorld> m_physXWorld;
    shared_ptr<PhysXWorld::TriTree> m_physXTriTree;
    TriTree m_g3dTriTree;

    App(const GApp::Settings& settings = GApp::Settings());

    virtual void onInit() override;
    virtual void onAI() override;
    virtual void onNetwork() override;
    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) override;
    virtual void onPose(Array<shared_ptr<Surface> >& posed3D, Array<shared_ptr<Surface2D> >& posed2D) override;

    // You can override onGraphics if you want more control over the rendering loop.
    // virtual void onGraphics(RenderDevice* rd, Array<shared_ptr<Surface> >& surface, Array<shared_ptr<Surface2D> >& surface2D) override;

    virtual void onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) override;
    virtual void onGraphics2D(RenderDevice* rd, Array<shared_ptr<Surface2D> >& surface2D) override;

    virtual bool onEvent(const GEvent& e) override;
    virtual void onUserInput(UserInput* ui) override;
    virtual void onCleanup() override;
    
    /** Sets m_endProgram to true. */
    virtual void endProgram();
};

#endif
