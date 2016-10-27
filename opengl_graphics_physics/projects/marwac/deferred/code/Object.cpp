#include "Object.h"
#include "Node.h"
#include "Material.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include <cmath> 
#include "OBJ.h"
#include "DebugDraw.h"
#include "PhysicsManager.h"
#include "FBOManager.h"
using namespace mwm;

Object::Object()
{
	//i should write default constructor for mesh and material and maybe texture but not required
	this->radius = 1;
	this->obb.color = Vector3(0.f, 0.8f, 0.8f);
	this->aabb.color = Vector3(1.f, 0.54f, 0.f);
	//inertia_tensor = Matrix3::identityMatrix();
	//true_innertia_tensor = Matrix3::identityMatrix();
}

Object::~Object()
{
}

void Object::drawLight(const Matrix4& Projection, const Matrix4& View, const Vector3& camPos)
{
	//apply transformation matrix from node
	Matrix4 offsetMatrix = Matrix4::translate(meshOffset);
	Matrix4 dModel = offsetMatrix*this->node.TopDownTransform;
	Matrix4F ViewMatrix = View.toFloat();
	Matrix4F MVP = (dModel*View*Projection).toFloat();
	GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();
	//vertex
	MatrixHandle = glGetUniformLocation(currentShaderID, "MVP");
	glUniformMatrix4fv(MatrixHandle, 1, GL_FALSE, &MVP[0][0]);
	
	//fragment
	ViewMatrixHandle = glGetUniformLocation(currentShaderID, "V");
	glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, &ViewMatrix[0][0]);

	GLuint LightPosHandle = glGetUniformLocation(currentShaderID, "LightPosition_worldspace");
	//light dir is set only for directional light once //or if many dir lights then i need to add invDir to the struct prob
	glUniform3fv(LightPosHandle, 1, &this->node.position.x);

	GLuint LightRadius = glGetUniformLocation(currentShaderID, "lightRadius");
	glUniform1f(LightRadius, this->node.scale.x);

	GLuint CameraPos = glGetUniformLocation(currentShaderID, "CameraPos");

	glUniform3fv(CameraPos, 1, &camPos.x);
	

	//this values are for the light, previously they were for objects so now i have to generate textures instead
	//and move those variables out
	//i probably wnat to keep and rename the color for light color and the specual value for shininess
	//ok let's do this

	GLuint LightPower = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "lightPower");
	glUniform1f(LightPower, this->mat->diffuseIntensity);

	GLuint LightColor = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "lightColor");
	glUniform3fv(LightColor, 1, &this->mat->color.x);

	//bind vao before drawing
	glBindVertexArray(this->mesh->vaoHandle);

	// Draw the triangles !
	glDrawElements(GL_TRIANGLES, this->mesh->indicesSize, GL_UNSIGNED_INT, (void*)0); // mode, count, type, element array buffer offset
}



void Object::drawGeometry(const Matrix4& Projection, const Matrix4& View)
{
	//apply transformation matrix from node
	Matrix4 offsetMatrix = Matrix4::translate(meshOffset);
	Matrix4 dModel = offsetMatrix*this->node.TopDownTransform;
	Matrix4F ModelMatrix = dModel.toFloat();
	Matrix4F MVP = (dModel*View*Projection).toFloat();
	GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();
	//vertex
	MatrixHandle = glGetUniformLocation(currentShaderID, "MVP");
	glUniformMatrix4fv(MatrixHandle, 1, GL_FALSE, &MVP[0][0]);

	ModelMatrixHandle = glGetUniformLocation(currentShaderID, "M");
	glUniformMatrix4fv(ModelMatrixHandle, 1, GL_FALSE, &ModelMatrix[0][0]);

	//below is only for geometry pass
	MaterialShininessValue = glGetUniformLocation(currentShaderID, "MaterialShininessValue");
	MaterialAmbientIntensityValueHandle = glGetUniformLocation(currentShaderID, "MaterialAmbientIntensityValue"); //ambient color for object
	MaterialSpecularColorHandle = glGetUniformLocation(currentShaderID, "MaterialSpecularColor"); //specular color for object
	MaterialDiffuseIntensityValueHandle = glGetUniformLocation(currentShaderID, "MaterialDiffuseIntensityValue"); //intensity of the diffuse color for object
	MaterialColorHandle = glGetUniformLocation(currentShaderID, "MaterialColor"); //diffuse color of the object
	glUniform1f(MaterialDiffuseIntensityValueHandle, this->mat->diffuseIntensity);
	glUniform1f(MaterialShininessValue, this->mat->shininess);
	glUniform1f(MaterialAmbientIntensityValueHandle, this->mat->ambientIntensity);
	glUniform3fv(MaterialSpecularColorHandle, 1, &this->mat->specular.x);
	glUniform3fv(MaterialColorHandle, 1, &this->mat->color.x);

	TextureSamplerHandle = glGetUniformLocation(currentShaderID, "myTextureSampler");
	//we bind texture for each object since it can be different 
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->mat->texture2D->TextureID);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(TextureSamplerHandle, 0);

	//bind vao before drawing
	glBindVertexArray(this->mesh->vaoHandle);

	// Draw the triangles !
	glDrawElements(GL_TRIANGLES, this->mesh->indicesSize, GL_UNSIGNED_INT, (void*)0); // mode, count, type, element array buffer offset
}


void Object::draw(const Matrix4& Projection, const Matrix4& View)
{
    //apply transformation matrix from node
	//we do physics around the center of the object
	//and in cases when pivot point is not in the center of the object 
	//we have to apply the offset for the graphics to match their physical position 
	Matrix4 offsetMatrix = Matrix4::translate(meshOffset); 

	Matrix4F ModelMatrix = (offsetMatrix*this->node.TopDownTransform).toFloat();
    Matrix4F ViewMatrix = View.toFloat();
	Matrix4F MVP = (offsetMatrix*this->node.TopDownTransform*View*Projection).toFloat();
	Matrix4F depthBiasMVP = (depthMVP*Matrix4::biasMatrix()).toFloat();
	GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();

	MatrixHandle = glGetUniformLocation(currentShaderID, "MVP");
	ViewMatrixHandle = glGetUniformLocation(currentShaderID, "V");
	DepthBiasMatrixHandle = glGetUniformLocation(currentShaderID, "DepthBiasMVP");
	

	MaterialAmbientIntensityValueHandle = glGetUniformLocation(currentShaderID, "MaterialAmbientIntensityValue");
	MaterialSpecularColorHandle = glGetUniformLocation(currentShaderID, "MaterialSpecularValue");
	MaterialDiffuseIntensityValueHandle = glGetUniformLocation(currentShaderID, "MaterialDiffuseIntensityValue");
	MaterialColorHandle = glGetUniformLocation(currentShaderID, "MaterialColorValue");
	PickingObjectIndexHandle = glGetUniformLocation(currentShaderID, "gObjectIndexVec4");
	TextureSamplerHandle = glGetUniformLocation(currentShaderID, "myTextureSampler");

    glUniformMatrix4fv(MatrixHandle, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(DepthBiasMatrixHandle, 1, GL_FALSE, &depthBiasMVP[0][0]);

	glUniform1f(MaterialAmbientIntensityValueHandle, this->mat->ambientIntensity);
	glUniform3fv(MaterialSpecularColorHandle, 1, &this->mat->specular.vect[0]);
    glUniform1f(MaterialDiffuseIntensityValueHandle, this->mat->diffuseIntensity);
	glUniform3fv(MaterialColorHandle, 1, &this->mat->color.vect[0]);

    int r = (this->ID & 0x000000FF) >>  0;
    int g = (this->ID & 0x0000FF00) >>  8;
    int b = (this->ID & 0x00FF0000) >> 16;

    glUniform4f(PickingObjectIndexHandle, r/255.0f, g/255.0f, b/255.0f, 1.0f);
    
	//we bind texture for each object since it can be different 
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->mat->texture2D->TextureID);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureSamplerHandle, 0);

    //bind vao before drawing
    glBindVertexArray(this->mesh->vaoHandle);

    // Draw the triangles !
    glDrawElements(GL_TRIANGLES, this->mesh->indicesSize, GL_UNSIGNED_INT, (void*)0); // mode, count, type, element array buffer offset

    //Unbind the VAO
    //glBindVertexArray(0);
}

void Object::drawDepth(const Matrix4& Projection, const Matrix4& View)
{
	//apply transformation matrix from node
	//we do physics around the center of the object
	//and in cases when pivot point is not in the center of the object 
	//we have to apply the offset for the graphics to match their physical position 
	Matrix4 offsetMatrix = Matrix4::translate(meshOffset);

	Matrix4F ModelMatrix = (offsetMatrix*this->node.TopDownTransform).toFloat();
	Matrix4F ViewMatrix = View.toFloat();
	depthMVP = offsetMatrix*this->node.TopDownTransform*View*Projection;
	Matrix4F MVP = (depthMVP).toFloat();

	GLuint depthMatrixHandle = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "depthMVP");

	glUniformMatrix4fv(depthMatrixHandle, 1, GL_FALSE, &MVP[0][0]);

	//bind vao before drawing
	glBindVertexArray(this->mesh->vaoHandle);

	// Draw the triangles !
	glDrawElements(GL_TRIANGLES, this->mesh->indicesSize, GL_UNSIGNED_INT, (void*)0); // mode, count, type, element array buffer offset

	//Unbind the VAO
	//glBindVertexArray(0);
}


void Object::AssignMaterial(Material* mat)
{
	this->mat = mat;
}

void Object::AssignMesh(Mesh* mesh)
{
	this->mesh = mesh;
	SetMeshOffset(mesh->obj->CenterOfMass()*-1.f);
	Vector3 extents = mesh->obj->dimensions*this->node.scale;
	this->obb.halfExtent = extents*0.5f;
	if (mass < FLT_MAX) SetInertiaTensor(Matrix3::CuboidInertiaTensor(this->mass, extents));
	else inverse_inertia_tensor = Matrix3();
}

void Object::setRadius(float radius)
{
	this->radius = radius;
}

Vector3 Object::extractScale()
{
	return node.TopDownTransform.extractScale();
}

Vector3 Object::getScale()
{
	return this->node.scale;
}

void Object::IntegrateMid(float timestep)
{
	if (!isAwake || isKinematic) return;
	
	this->acceleration = PhysicsManager::Instance()->gravity + this->accum_force * this->massInverse;

	this->angular_acc = inverse_inertia_tensor_world*this->accum_torque;

	//acceleration is 
	//change in velocity / change in time
	Vector3 oldVelocity = this->velocity;
	Vector3 changeInVel = acceleration * timestep;
	this->velocity += changeInVel; 

	Vector3 oldAngularVel = this->angular_velocity;
	this->angular_velocity += angular_acc * timestep;

	this->velocity *= pow(damping, timestep); 
	this->angular_velocity *= pow(damping, timestep);

	//midpoint	
	Vector3 changeInPos = 0.5f*(this->velocity + oldVelocity)*timestep;	

	this->Translate(changeInPos);

	Vector3 axis = this->angular_velocity.vectNormalize();
	float angle = 0.5f*(this->angular_velocity.vectLengt() + oldAngularVel.vectLengt());
	if (angle != 0){
		Quaternion test(angle * timestep, axis);
		SetOrientation((test*GetOrientation()).Normalized());
	}

	//clear force
	this->accum_force = Vector3(0.f, 0.f, 0.f);
	this->accum_torque = Vector3(0.f, 0.f, 0.f);

	UpdateKineticEnergyStoreAndPutToSleep(timestep);

}

void Object::IntegrateMid2(float timestep)
{
	if (!isAwake || isKinematic) return;
	
	this->acceleration = PhysicsManager::Instance()->gravity + this->accum_force * this->massInverse;

	this->angular_acc = inverse_inertia_tensor_world*this->accum_torque;

	//acceleration is 
	//change in velocity / change in time
	Vector3 changeInVel = acceleration * (timestep * 0.5f); //half-step
	Vector3 changeInVel2 = (acceleration + changeInVel) * timestep; // midpoint result
	this->velocity += changeInVel2;

	Vector3 changeInAngVel = angular_acc * (timestep * 0.5f); //half-step
	Vector3 changeInAngVel2 = (angular_acc + changeInAngVel) * timestep; // midpoint result
	this->angular_velocity += changeInAngVel2;

	this->velocity *= pow(damping, timestep);
	this->angular_velocity *= pow(damping, timestep);

	//midpoint	
	Vector3 changeInPos = this->velocity*timestep;

	this->Translate(changeInPos);

	Vector3 axis = this->angular_velocity.vectNormalize();
	float angle = this->angular_velocity.vectLengt();
	if (angle != 0){
		Quaternion test(angle * timestep, axis);
		SetOrientation((test*GetOrientation()).Normalized());
	}

	//clear force
	this->accum_force = Vector3(0.f, 0.f, 0.f);
	this->accum_torque = Vector3(0.f, 0.f, 0.f);

	UpdateKineticEnergyStoreAndPutToSleep(timestep);

}

void Object::IntegrateEuler(float timestep)
{
	if (!isAwake || isKinematic) return;

	this->acceleration = PhysicsManager::Instance()->gravity + this->accum_force * this->massInverse;

	this->angular_acc = inverse_inertia_tensor_world*this->accum_torque;

	//acceleration is 
	//change in velocity / change in time
	Vector3 changeInVel = acceleration * timestep;
	this->velocity += changeInVel; 

	this->angular_velocity += angular_acc * timestep;

	this->velocity *= pow(damping, timestep); 
	this->angular_velocity *= pow(damping, timestep);

	Vector3 changeInPos = this->velocity * timestep;
	
	this->Translate(changeInPos);

	Vector3 axis = this->angular_velocity.vectNormalize();
	float angle = this->angular_velocity.vectLengt();
	if (angle != 0){
		Quaternion test(angle * timestep, axis);
		//angularvel * quaternion
		SetOrientation((test*GetOrientation()).Normalized());
	}

	//clear force
	this->accum_force = Vector3(0.f, 0.f, 0.f);
	this->accum_torque = Vector3(0.f, 0.f, 0.f);

	UpdateKineticEnergyStoreAndPutToSleep(timestep);

	//UpdateTransforms();
}

void Object::ApplyImpulse(const Vector3& force, const Vector3& picking_point)
{
	setAwake();
	this->accum_force += force;
	DebugDraw::Instance()->DrawShapeAtPos("cube", picking_point);
	//DebugDraw::Instance()->DrawShapeAtPos("pyramid", picking_point+force);
	Vector3 directionFromCenterToPickingPoint = picking_point - GetPosition();
	Vector3 torque = directionFromCenterToPickingPoint.crossProd(force);
	this->accum_torque += torque;
}

void Object::ApplyImpulse(const Vector3& direction, float magnitude, const Vector3& point)
{
	setAwake();
	this->accum_force += direction*magnitude;
	DebugDraw::Instance()->DrawShapeAtPos("cube", point);
	//DebugDraw::Instance()->DrawShapeAtPos("pyramid", picking_point+force);
	Vector3 directionFromCenterToPickingPoint = point - GetPosition();
	Vector3 torque = directionFromCenterToPickingPoint.crossProd(direction);
	this->accum_torque += torque*magnitude;
}

Vector3 Object::ConvertPointToWorld(const Vector3& point, const Matrix4& modelTransform)
{
	Vector4 pointV4 = Vector3::vec3TOvec4(point, 1.f);
	Vector3 pointInWorld = (modelTransform * pointV4).get_xyz();
	return pointInWorld;
}

void Object::SetPosition( float x, float y, float z )
{
	this->node.position = Vector3(x, y, z);
}


void Object::SetPosition(const Vector3& vector )
{
	this->node.position = vector;
}

Vector3 Object::GetPosition() const
{
	return this->node.position;
}


void Object::SetScale(float x, float y, float z)
{
	this->node.scale = Vector3(x, y, z);
	Vector3 extents = mesh->obj->dimensions*this->node.scale;
	this->obb.halfExtent = extents*0.5f;
	SetMass(this->mass);
}


void Object::SetScale(const Vector3& vector )
{
	this->node.scale = vector;
	Vector3 extents = mesh->obj->dimensions*this->node.scale;
	this->obb.halfExtent = extents*0.5f;
	SetMass(this->mass);
}

void Object::Translate(float x, float y, float z)
{
	this->node.position += Vector3(x, y, z);
}

void Object::Translate(const Vector3& vector)
{
	this->node.position += vector;
}

void Object::SetInertiaTensor(const Matrix3& I)
{
	this->inverse_inertia_tensor = I.inverse();
}

float Object::GetMass()
{
	return this->mass;
}

float Object::GetMassInverse()
{
	return this->massInverse;
}

void Object::SetMass(float mass)
{
	this->mass = mass;
	if (mass < FLT_MAX)	{
		this->massInverse = 1.f / mass;
		SetInertiaTensor(Matrix3::CuboidInertiaTensor(this->mass, mesh->obj->dimensions*this->node.scale));
	}
	else {
		this->massInverse = 0.f;
		//inverse_inertia_tensor = Matrix3();
	}
}

Vector3 Object::GetMeshDimensions()
{
	return this->mesh->obj->GetDimensions();
}

void Object::SetOrientation(const Quaternion& q)
{
	this->node.orientation = q;
}

Quaternion Object::GetOrientation()
{
	return this->node.orientation;
}

void Object::SetMeshOffset(const Vector3& offset)
{
	meshOffset = offset;
}

//should be added in the integrate at the end
void Object::UpdateKineticEnergyStoreAndPutToSleep(float timestep)
{
	// Update the kinetic energy store, and possibly put the body to
	// sleep.
	if (canSleep) {

		float currentMotion = velocity.dotAKAscalar(velocity) + angular_velocity.dotAKAscalar(angular_velocity);
		float bias = powf(0.5f, timestep);
		motion = bias*motion + (1.f - bias)*currentMotion;

		if (motion < sleepEpsilon) setAwake(false);
		else if (motion > 10.f * sleepEpsilon) motion = 10.f * sleepEpsilon;
	}
}

void Object::UpdateBoundingBoxes()
{
	//OBB
	obb.model = node.orientation.ConvertToMatrix3(); //don't let the model contain the scale
	obb.mm = DebugDraw::Instance()->boundingBox.CalcValuesInWorld(Matrix3::scale(node.scale*mesh->obj->dimensions) * obb.model, node.position);

	//AABB
	aabb.model = Matrix4::scale((obb.mm.max - obb.mm.min))*Matrix4::translate(node.position);
}

void Object::UpdateInertiaTensor()
{
	inverse_inertia_tensor_world = obb.model * this->inverse_inertia_tensor * (~obb.model);
}

void Object::UpdatePosAndOrient(float timestep)
{
	Vector3 changeInPos = this->velocity * timestep;

	this->Translate(changeInPos);

	Vector3 axis = this->angular_velocity.vectNormalize();
	float angle = this->angular_velocity.vectLengt();
	if (angle != 0){
		Quaternion test(angle * timestep, axis);
		//angularvel * quaternion
		SetOrientation((test*GetOrientation()).Normalized());
	}
}

void Object::SetOBBHalfExtent(const Vector3& scale)
{
	obb.halfExtent = scale;
}

void Object::IntegrateRunge2(float timestep)
{
	if (!isAwake || isKinematic) return;

	this->acceleration = PhysicsManager::Instance()->gravity + this->accum_force * this->massInverse;

	this->angular_acc = inverse_inertia_tensor_world*this->accum_torque;

	//runge kutta
	//first step is euler
	Vector3 changeInVel1 = acceleration; //this is k1 for vel for time 0 prev acc + force
	Vector3 changeInAng1 = angular_acc; //this is k1 for ang for time 0 prev acc + force

	Vector3 changeInVel2 = acceleration + (timestep / 2.0f) * changeInVel1; //k2 vel
	Vector3 changeInAng2 = angular_acc + (timestep / 2.0f) * changeInAng1; //k2 ang

	Vector3 changeInVel3 = acceleration + (timestep / 2.0f) * changeInVel2; //k3 vel
	Vector3 changeInAng3 = angular_acc + (timestep / 2.0f) * changeInAng2; //k3 ang

	Vector3 changeInVel4 = acceleration + timestep * changeInVel3; //k4 vel
	Vector3 changeInAng4 = angular_acc + timestep * changeInAng3; //k4 ang

	Vector3 changeInVelF = (timestep / 6.0f) * (changeInVel1 + 2.0f*(changeInVel2 + changeInVel3) + changeInVel4);
	Vector3 changeInAngF = (timestep / 6.0f) * (changeInAng1 + 2.0f*(changeInAng2 + changeInAng3) + changeInAng4);

	this->velocity *= pow(damping, timestep);
	this->angular_velocity *= pow(damping, timestep);

	this->velocity += changeInVelF;
	this->angular_velocity += changeInAngF;

	//clear force
	this->accum_force = Vector3(0.f, 0.f, 0.f);
	this->accum_torque = Vector3(0.f, 0.f, 0.f);

	UpdateKineticEnergyStoreAndPutToSleep(timestep);

}

void Object::IntegrateRunge3(float timestep)
{
	if (!isAwake || isKinematic) return;

	this->acceleration = PhysicsManager::Instance()->gravity + this->accum_force * this->massInverse;

	this->angular_acc = inverse_inertia_tensor_world*this->accum_torque;

	Vector3 dx1 = velocity;
	Vector3 dv1 = acceleration;
	// step 2
	Vector3 dx2 = velocity + (timestep / 2.0f) * dx1;
	Vector3 dv2 = acceleration + (timestep / 2.0f) * dv1; //k2 vel
	// step 3
	Vector3 dx3 = velocity + (timestep / 2.0f) * dx2;
	Vector3 dv3 = acceleration + (timestep / 2.0f) * dv2; //k3 vel
	// step 4
	Vector3 dx4 = velocity + timestep * dx3;
	Vector3 dv4 = acceleration + timestep * dv3; //k4 vel
	// now combine the derivative estimates and
	// compute new state
	Vector3 changeInPos = (timestep / 6.0f) * (dx1 + (dx2 + dx3) * 2.0f + dx4);
	this->Translate(changeInPos);

	this->velocity += (timestep / 6.0f) * (dv1 + (dv2 + dv3) * 2.0f + dv4);
	this->velocity *= pow(damping, timestep);

	//orientation
	Vector3 adx1 = angular_velocity;
	Vector3 adv1 = angular_acc;
	// step 2
	Vector3 adx2 = angular_velocity + (timestep / 2.0f) * adx1;
	Vector3 adv2 = angular_acc + (timestep / 2.0f) * adv1; //k2 ang
	// step 3
	Vector3 adx3 = angular_velocity + (timestep / 2.0f) * adx2;
	Vector3 adv3 = angular_acc + (timestep / 2.0f) * adv2; //k3 ang
	// step 4
	Vector3 adx4 = angular_velocity + timestep * adx3;
	Vector3 adv4 = angular_acc + timestep * adv3; //k4 ang
	// now combine the derivative estimates and
	// compute new state

	Vector3 changeInRot = (timestep / 6.0f) * (adx1 + (adx2 + adx3) * 2.0f + adx4);
	
	this->angular_velocity += (timestep / 6.0f) * (adv1 + adv2 * 2.0f + adv3 * 2.0f + adv4);
	this->angular_velocity *= pow(damping, timestep);

	Vector3 axis = changeInRot.vectNormalize();
	float angle = changeInRot.vectLengt();
	if (angle != 0){
		Quaternion test(angle, axis);
		SetOrientation((test*GetOrientation()).Normalized());
	}

	//clear force
	this->accum_force = Vector3(0.f, 0.f, 0.f);
	this->accum_torque = Vector3(0.f, 0.f, 0.f);

	UpdateKineticEnergyStoreAndPutToSleep(timestep);

}

void Object::setAwake(const bool awake)
{
	if (awake) {
		isAwake = true;

		// Add a bit of motion to avoid it falling asleep immediately.
		motion = this->sleepEpsilon*2.0f;
		this->obb.color = Vector3(0.f, 0.8f, 0.8f);
	}
	else {
		isAwake = false;
		velocity = Vector3(0.f, 0.f, 0.f);
		angular_velocity = Vector3(0.f, 0.f, 0.f);
		this->obb.color = Vector3(2.0f, 0.0f, 0.0f);
	}
}

void Object::setCanSleep(const bool canSleep)
{
	this->canSleep = canSleep;

	if (!canSleep && !isAwake) setAwake();
}
