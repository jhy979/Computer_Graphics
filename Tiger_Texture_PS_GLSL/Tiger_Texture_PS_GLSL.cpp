#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>

#include "Shaders/LoadShaders.h"
#include "My_Shading.h"

#include "my_geom_objects.h"

typedef struct Camera {
	float pos[3];
	float u[3], v[3], n[3];
	float fovy, aspect_ration, zNear, zFar;

}Camera;
Camera camera;
// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewMatrix, ProjectionMatrix;
//아이언맨 모델뷰 매트릭스
glm::mat4 ModelView_ironman;

//줌인줌아웃인자
static float fov = 45.0f;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f




// lights in scene
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

// fog stuffs
// you could control the fog parameters interactively: FOG_COLOR, FOG_NEAR_DISTANCE, FOG_FAR_DISTANCE   
int flag_fog;


// axes object



// callbacks
float PRP_distance_scale[6] = { 0.5f, 1.0f, 2.5f, 5.0f, 10.0f, 20.0f };

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(50.0f, 50.0f, 50.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes();
	glLineWidth(1.0f);

	glUseProgram(h_ShaderProgram_TXPS);
	set_material_floor();
	glUniform1i(loc_texture, TEXTURE_ID_FLOOR);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-500.0f, 0.0f, 500.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1000.0f, 1000.0f, 1000.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_floor();
	
	//사람 그리기
	set_material_ben();
	glUniform1i(loc_texture, TEXTURE_ID_BEN);
	float person_timer = timestamp_scene % 700;
	if (funny_effect == 0) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(280.0f - person_timer - fabs(10 * sinf(person_timer / 15)), fabs(5 * sinf(person_timer / 10)), 280.0f - person_timer - fabs(10 * sinf(person_timer / 10))));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -120 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, -100.0f, -100.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_ben();
	}
	else {
		for (int i = 0; i < 3; i++) {
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(280.0f -5*i- person_timer / 70 * 40 - fabs(10 * sinf(person_timer / 15)), person_timer, 280.0f - 5 * i - person_timer / 70 * 40) - fabs(10 * sinf(person_timer / 21)));
			ModelViewMatrix = glm::rotate(ModelViewMatrix, -120 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelViewMatrix = glm::rotate(ModelViewMatrix, -(person_timer / 70 * 9) * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
			ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, -100.0f, -100.0f));
			ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(person_timer / 700 * 10, person_timer / 700 * 10, person_timer / 700 * 10));
			ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
			ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

			glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
			glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
			draw_ben();
		}

	}
	//울프 그리기
	set_material_wolf();
	glUniform1i(loc_texture, TEXTURE_ID_WOLF);
	float rotation_angle_wolf = timestamp_scene % 200;
	for (int i = 0; i < 8; i++) {
		if (i == 0)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, rotation_angle_wolf * 3));
		else if (i == 1)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(rotation_angle_wolf * 3, 0.0f, rotation_angle_wolf * 3));
		else if (i == 2)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(rotation_angle_wolf * 3, 0.0f, 0.0f));
		else if (i == 3)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(rotation_angle_wolf * 3, 0.0f, -rotation_angle_wolf * 3));
		else if (i == 4)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -rotation_angle_wolf * 3));
		else if (i == 5)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-rotation_angle_wolf * 3, 0.0f, -rotation_angle_wolf * 3));
		else if (i == 6)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-rotation_angle_wolf * 3, 0.0f, 0.0f));
		else if (i == 7)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-rotation_angle_wolf * 3, 0.0f, rotation_angle_wolf * 3));

		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, rotation_angle_wolf, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 45 * i * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -25 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_wolf();
	}


	//거미그리기
	set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_SPIDER);
	double translate_spider = timestamp_scene % 700;

	if (translate_spider <= 80) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(300.0f, translate_spider + 40, 500.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -50.0f, 50.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1.0f, 1.0f, -1.0f));
	}
	else if (translate_spider <= 170) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(300.0f - ((translate_spider - 80) / 6), 120 + (translate_spider - 80) / 6, 500.0f - ((translate_spider - 80) / 3)));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -((int)translate_spider - 80) * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -50.0f, 50.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1.0f, 1.0f, -1.0f));
	}
	else if (translate_spider < 500) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(285.0f + (translate_spider - 170) / 3, 135.0f, 470.0f - (translate_spider - 170)));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -50.0f, 50.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1.0f, 1.0f, -1.0f));
	}

	else if (translate_spider < 590) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(395, 135.0f - (translate_spider - 500) / 3, 140.0f - (translate_spider - 500) / 3));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -((int)translate_spider - 500) * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -50.0f, 50.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1.0f, 1.0f, -1.0f));
	}
	else if (translate_spider < 660) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(395.0f, 105.0f - (translate_spider - 590), 110.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -50.0f, 50.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1.0f, 1.0f, -1.0f));
	}

	else if (translate_spider < 700) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(395.0f, 35.0f, 110.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -50.0f, 50.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1-(translate_spider - 660) / 40, 1-(translate_spider - 660) / 40,1-(translate_spider - 660) / 40));

	}
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_spider();


	//옵티머스 그리기

	set_material_optimus();
	glUniform1i(loc_texture, TEXTURE_ID_OPTIMUS);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-200.0, 0.0f, 200.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, (-90.0f + sinf((float)timestamp_scene / 10)) * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.4f, 0.5f, 0.4f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	//스크린효과

	draw_optimus();


	//버스그리기

	set_material_bus();
	glUniform1i(loc_texture, TEXTURE_ID_BUS);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(300.0f, 20.0f, 300.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -20 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(15.0f, 15.0f, 15.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	
	glUniform1i(loc_screen_effect, flag_screen_effect);
	draw_bus();
	glUniform1i(loc_screen_effect, 0);

	set_material_ironman();
	glUniform1i(loc_texture, TEXTURE_ID_IRONMAN);
	//아이언맨 그리기
	float rotation_angle_iron_man = timestamp_scene - 200.0f;
	
	if (rotation_angle_iron_man < 0)
	{
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(200.0f, 200 + rotation_angle_iron_man, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, (100 + rotation_angle_iron_man) * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, (30 - rotation_angle_iron_man / 10 * 3) * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, (20 - rotation_angle_iron_man / 10 * 2) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -rotation_angle_iron_man * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(30.0f, 30.0f, 30.0f));
	}
	else if (((int)rotation_angle_iron_man / 360) % 2 == 1) {
		ModelViewMatrix = glm::rotate(ViewMatrix, -rotation_angle_iron_man * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f + cosf(rotation_angle_iron_man) / 2, 200.0f + cosf(rotation_angle_iron_man) / 2, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 100 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 30 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 40 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(30.0f, 30.0f, 30.0f));
	}
	else {
		ModelViewMatrix = glm::rotate(ViewMatrix, -rotation_angle_iron_man * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f + cosf(rotation_angle_iron_man) / 2, 200.0f + cosf(rotation_angle_iron_man) / 2, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 100 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 30 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 40 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -5 * rotation_angle_iron_man * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(30.0f, 30.0f, 30.0f));


	}

	ModelView_ironman = ModelViewMatrix;


	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_ironman();


	glm::vec4 position_EC = ModelView_ironman * glm::vec4(light[3].position[0], light[3].position[1],
		light[3].position[2], light[3].position[3]);
	glUniform4fv(loc_light[3].position, 1, &position_EC[0]);
	glm::vec4 direction_EC = glm::mat4(ViewMatrix) * glm::vec4(light[3].spot_direction[0],
		light[3].spot_direction[1], light[3].spot_direction[2],1.0f);
	glUniform3fv(loc_light[3].spot_direction, 1, &direction_EC[0]);


	glUseProgram(0);

	glutSwapBuffers();
}

void timer_scene(int value) {
	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;
	cur_frame_tiger = timestamp_scene % N_TIGER_FRAMES;
	cur_frame_ben = timestamp_scene % N_BEN_FRAMES;
	cur_frame_wolf = timestamp_scene % N_WOLF_FRAMES;
	cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;

	rotation_angle_tiger = (timestamp_scene % 360) * TO_RADIAN;
	glutPostRedisplay();
	if (flag_tiger_animation)
		glutTimerFunc(10, timer_scene, 0);
}


void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0;
	static int PRP_distance_level = 4;
	glm::mat3 Rotation_Matrix;
	glm::vec3 changed_dir;
	glm::vec4 position_EC;
	glm::vec3 direction_EC;
	static int flag_blind_effect = 0;

	if ((key >= '0') && (key <= '0' + NUMBER_OF_LIGHT_SUPPORTED - 1)) {
		int light_ID = (int)(key - '0');

		glUseProgram(h_ShaderProgram_TXPS);
		light[light_ID].light_on = 1 - light[light_ID].light_on;
		glUniform1i(loc_light[light_ID].light_on, light[light_ID].light_on);
		glUseProgram(0);

		glutPostRedisplay();
		return;
	}

	switch (key) {
	case 'a': // toggle the animation effect.
		flag_tiger_animation = 1 - flag_tiger_animation;
		if (flag_tiger_animation) {
			glutTimerFunc(100, timer_scene, 0);
			fprintf(stdout, "^^^ Animation mode ON.\n");
		}
		else
			fprintf(stdout, "^^^ Animation mode OFF.\n");
		break;
	case 'f':
		flag_fog = 1 - flag_fog;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_fog, flag_fog);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 't':
	
		flag_choice_mapping = 1 - flag_choice_mapping;
		prepare_ironman();
		glutPostRedisplay();
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		}
		break;
	case 'd':
		PRP_distance_level = (PRP_distance_level + 1) % 6;
		fprintf(stdout, "^^^ Distance level = %d.\n", PRP_distance_level);

		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		
		break;
	case 'p':
		flag_polygon_fill = 1 - flag_polygon_fill;
		if (flag_polygon_fill)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
		break;
		//카메라 프레임의 각축 방향으로 이동
	case 'u':
		camera.pos[0] = camera.pos[0] + 10.0f;
		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		break;

	case 'i':
		camera.pos[0] = camera.pos[0] - 10.0f;
		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		break;
	case 'v':
		camera.pos[1] = camera.pos[1] + 10.0f;
		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		break;

	case 'b':
		camera.pos[1] = camera.pos[1] - 10.0f;
		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		break;

	case 'n':
		camera.pos[2] = camera.pos[2] + 10.0f;
		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		break;

	case 'm':
		camera.pos[2] = camera.pos[2] - 10.0f;
		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		break;
	case 'q':
		
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
		glUseProgram(0);
		break;
	case 'w':
		flag_blind_effect = 1 - flag_blind_effect;
	
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_blind_effect, flag_blind_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'r':
		funny_effect = 1 - funny_effect;
		prepare_optimus();
		prepare_bus();
		prepare_floor();
		prepare_wolf();
		prepare_spider();
		prepare_ben();
		if (funny_effect == 1) {
			camera.pos[0] = 0.0f;
			camera.pos[1] = 700.0f;
			camera.pos[2] = 0.0f;
			camera.v[0] = 0.0f;
			camera.v[1] = 1.0f;
			camera.v[2] = 0.0f;
			camera.n[0] = -100.0f;
			camera.n[1] = 500.0f;
			camera.n[2] = 0.0f;
			camera.u[0] = camera.v[1] * camera.n[2] - camera.v[2] * camera.n[1];
			camera.u[1] = camera.v[2] * camera.n[0] - camera.v[0] * camera.n[2];
			camera.u[2] = camera.v[0] * camera.n[1] - camera.v[0] * camera.n[1];
		
			ViewMatrix = glm::lookAt(PRP_distance_scale[2] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
				glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));

		}
		else {
			camera.pos[0] = 500.0f;
			camera.pos[1] = 300.0f;
			camera.pos[2] = 500.0f;
			camera.v[0] = 0.0f;
			camera.v[1] = 1.0f;
			camera.v[2] = 0.0f;
			camera.n[0] = 500.0f;
			camera.n[1] = 300.0f;
			camera.n[2] = 500.0f;
			camera.u[0] = camera.v[1] * camera.n[2] - camera.v[2] * camera.n[1];
			camera.u[1] = camera.v[2] * camera.n[0] - camera.v[0] * camera.n[2];
			camera.u[2] = camera.v[0] * camera.n[1] - camera.v[0] * camera.n[1];

			ViewMatrix = glm::lookAt(PRP_distance_scale[0] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
				glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));

		}
		display();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


		glutPostRedisplay();

		//스크린 효과
	case 'e':
			screen_frequency = screen_frequency +1.0f;
			if (screen_frequency > 60.0f)
				screen_frequency = 1.0f;
			glUseProgram(h_ShaderProgram_TXPS);
			glUniform1f(loc_screen_frequency, screen_frequency);
			glUseProgram(0);
			glutPostRedisplay();
		break;

	//u,v,n축 둘레 회전
	case ',':

		Rotation_Matrix = glm::mat3(glm::rotate(glm::mat4(1.0), 5*TO_RADIAN, glm::normalize(glm::vec3(camera.u[0], camera.u[1], camera.u[2]))));

		changed_dir = Rotation_Matrix * glm::vec3(camera.v[0], camera.v[1], camera.v[2]);
		camera.v[0] = changed_dir.x; camera.v[1] = changed_dir.y; camera.v[2] = changed_dir.z;

		changed_dir = Rotation_Matrix * glm::vec3(camera.n[0], camera.n[1], camera.n[2]);
		camera.n[0] = changed_dir.x; camera.n[1] = changed_dir.y; camera.n[2] = changed_dir.z;

		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		glutPostRedisplay();
		break;

	case '.':
		Rotation_Matrix = glm::mat3(glm::rotate(glm::mat4(1.0),5*TO_RADIAN,glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2]))));
		
		changed_dir = Rotation_Matrix * glm::vec3(camera.u[0], camera.u[1], camera.u[2]);
		camera.u[0] = changed_dir.x; camera.u[1] = changed_dir.y; camera.u[2] = changed_dir.z;

		changed_dir = Rotation_Matrix * glm::vec3(camera.n[0], camera.n[1], camera.n[2]);
		camera.n[0] = changed_dir.x; camera.n[1] = changed_dir.y; camera.n[2] = changed_dir.z;

		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		glutPostRedisplay();
		break;

	case '/':
		Rotation_Matrix = glm::mat3(glm::rotate(glm::mat4(1.0), 5 * TO_RADIAN, glm::normalize(glm::vec3(camera.n[0], camera.n[1], camera.n[2]))));

		changed_dir = Rotation_Matrix * glm::vec3(camera.u[0], camera.u[1], camera.u[2]);
		camera.u[0] = changed_dir.x; camera.u[1] = changed_dir.y; camera.u[2] = changed_dir.z;

		changed_dir = Rotation_Matrix * glm::vec3(camera.v[0], camera.v[1], camera.v[2]);
		camera.v[0] = changed_dir.x; camera.v[1] = changed_dir.y; camera.v[2] = changed_dir.z;

		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));		glutPostRedisplay();
		break;

	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups
		break;
	}
	glUseProgram(h_ShaderProgram_TXPS);

	// Must update the light 1's geometry in EC.
	position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]);
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0],
		light[1].spot_direction[1], light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]);

	// Must update the light 2's geometry in EC.
	position_EC = ViewMatrix * glm::vec4(light[2].position[0], light[2].position[1],
		light[2].position[2], light[2].position[3]);
	glUniform4fv(loc_light[2].position, 1, &position_EC[0]);
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[2].spot_direction[0],
		light[2].spot_direction[1], light[2].spot_direction[2]);
	glUniform3fv(loc_light[2].spot_direction, 1, &direction_EC[0]);

	glUseProgram(0);
}

//줌인줌아웃, 카메라 위치 초기화
void Keyboard_special(int key, int x, int y)
{
	float aspect_ratio = (float)global_width / global_height;
	switch (key)
	{
	case GLUT_KEY_PAGE_DOWN:
	
		if (fov >= 1.0f && fov <= 100.0f)
		{
			fov = fov + 1.0f;
		
		}

		glViewport(0, 0, global_width, global_height);

		ProjectionMatrix = glm::perspective(fov * TO_RADIAN, aspect_ratio, 100.0f, 20000.0f);

		glutPostRedisplay();
		break;

	case GLUT_KEY_PAGE_UP:
		if (fov >= 2.0f)
			fov -= 1.0f;

		glViewport(0, 0, global_width, global_height);

		ProjectionMatrix = glm::perspective(fov * TO_RADIAN, aspect_ratio, 100.0f, 20000.0f);

		glutPostRedisplay();

		glutPostRedisplay();
		break;

		//이건 그냥 시점 초기화
	case GLUT_KEY_F1:
		camera.pos[0] = 500.0f;
		camera.pos[1] = 300.0f;
		camera.pos[2] = 500.0f;
		camera.v[0] = 0.0f;
		camera.v[1] = 1.0f;
		camera.v[2] = 0.0f;
		camera.n[0] = 500.0f;
		camera.n[1] = 300.0f;
		camera.n[2] = 500.0f;
		camera.u[0] = camera.v[1] * camera.n[2] - camera.v[2] * camera.n[1];
		camera.u[1] = camera.v[2] * camera.n[0] - camera.v[0] * camera.n[2];
		camera.u[2] = camera.v[0] * camera.n[1] - camera.v[0] * camera.n[1];

		ViewMatrix = glm::lookAt(PRP_distance_scale[0] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
			glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::normalize(glm::vec3(camera.v[0], camera.v[1], camera.v[2])));
		glViewport(0, 0, global_width, global_height);
		fov = 45;
		ProjectionMatrix = glm::perspective(fov * TO_RADIAN, aspect_ratio, 100.0f, 20000.0f);

		glutPostRedisplay();
		break;
	}
}

void reshape(int width, int height) {
	
	float aspect_ratio;
	global_height = height;
	global_width = width;
	glViewport(0, 0, width, height);

	aspect_ratio = (float)width / height;
	ProjectionMatrix = glm::perspective(fov * TO_RADIAN, aspect_ratio, 100.0f, 20000.0f);

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &rectangle_VAO);
	glDeleteBuffers(1, &rectangle_VBO);

	glDeleteVertexArrays(1, &tiger_VAO);
	glDeleteBuffers(1, &tiger_VBO);

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(Keyboard_special);
	glutReshapeFunc(reshape);	
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	
	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");

	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");
	//스크린 효과
	loc_screen_effect = glGetUniformLocation(h_ShaderProgram_TXPS, "screen_effect");
	loc_screen_frequency = glGetUniformLocation(h_ShaderProgram_TXPS, "screen_frequency");
	loc_screen_width = glGetUniformLocation(h_ShaderProgram_TXPS, "screen_width");
	//스폿광원 블라인드
	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_TXPS, "u_blind_effect");
}

void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	//스크린효과
	glUniform1i(loc_screen_effect, 0);
	glUniform1f(loc_screen_frequency, 1.0f);
	glUniform1f(loc_screen_width, 0.1f);

	//스폿광원효과
	glUniform1i(loc_blind_effect, 0);
	glUseProgram(0);
}

void initialize_flags(void) {
	flag_tiger_animation = 1;
	flag_polygon_fill = 1;
	flag_texture_mapping = 1;
	flag_fog = 0;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_flag_fog, flag_fog);
	glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
	glUseProgram(0);
}

void initialize_OpenGL(void) {

	glEnable(GL_MULTISAMPLE);


	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	camera.pos[0] = 500.0f;
	camera.pos[1] = 300.0f;
	camera.pos[2] = 500.0f;

	camera.v[0] = 0.0f;
	camera.v[1] = 1.0f;
	camera.v[2] = 0.0f;

	camera.n[0] = 500.0f;
	camera.n[1] = 300.0f;
	camera.n[2] = 500.0f;


	camera.u[0] = camera.v[1] * camera.n[2] - camera.v[2] * camera.n[1];
	camera.u[1] = camera.v[2] * camera.n[0] - camera.v[0] * camera.n[2];
	camera.u[2] = camera.v[0] * camera.n[1] - camera.v[0] * camera.n[1];
	
	ViewMatrix = glm::lookAt(PRP_distance_scale[0] * glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
		glm::vec3(-camera.n[0], -camera.n[1], -camera.n[2]), glm::vec3(camera.v[0], camera.v[1], camera.v[2]));
	
	//이게 왜있는거지?
	//ViewMatrix = glm::lookAt(4.0f / 6.0f * glm::vec3(500.0f, 600.0f, 500.0f),glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	initialize_lights_and_material();
	initialize_flags();

	glGenTextures(N_TEXTURES_USED, texture_names);
}

void set_up_scene_lights(void) {
	// point_light_EC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 500.0f; 	// point light position in EC
	light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.3f; light[0].ambient_color[1] = 0.3f;
	light[0].ambient_color[2] = 0.3f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.3f; light[0].diffuse_color[1] = 0.3f;
	light[0].diffuse_color[2] = 0.3f; light[0].diffuse_color[3] = 1.0f;

	light[0].specular_color[0] = 0.3f; light[0].specular_color[1] = 0.3f;
	light[0].specular_color[2] = 0.3f; light[0].specular_color[3] = 1.0f;

	// spot_light_WC: use light 1
	light[1].light_on = 1;
	light[1].position[0] = -200.0f; light[1].position[1] = 500.0f; // spot light position in WC
	light[1].position[2] = 200.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.0f; light[1].ambient_color[1] = 0.8f;
	light[1].ambient_color[2] = 1.0f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = .0f; light[1].diffuse_color[1] = 0.6f;
	light[1].diffuse_color[2] = 1.0f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.772f; light[1].specular_color[1] = 0.772f;
	light[1].specular_color[2] = 0.772f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light[1].spot_direction[2] = 0.0f;
	light[1].spot_cutoff_angle = 20.0f;
	light[1].spot_exponent = 8.0f;

	//spot_light_WC: use light 2
	light[2].light_on = 0;
	light[2].position[0] = 300.0f; light[2].position[1] = 500.0f; // spot light position in WC
	light[2].position[2] = 300.0f; light[2].position[3] = 1.0f;

	light[2].ambient_color[0] = 1.0f; light[2].ambient_color[1] = 0.0f;
	light[2].ambient_color[2] = 0.0f; light[2].ambient_color[3] = 1.0f;

	light[2].diffuse_color[0] = 1.0f; light[2].diffuse_color[1] = 0.1f;
	light[2].diffuse_color[2] = 0.2f; light[2].diffuse_color[3] = 1.0f;

	light[2].specular_color[0] = 1.0f; light[2].specular_color[1] = 0.4f;
	light[2].specular_color[2] = 0.0f; light[2].specular_color[3] = 1.0f;

	light[2].spot_direction[0] = 0.0f; light[2].spot_direction[1] = -1.0f; // spot light direction in WC
	light[2].spot_direction[2] = 0.0f;
	light[2].spot_cutoff_angle = 15.0f;
	light[2].spot_exponent = 5.0f;

	//spot_light_WC: use light 3

	light[3].light_on = 0;

	light[3].position[0] = 1.5f; light[3].position[1] = 10.0f; // spot light position in WC
	light[3].position[2] = 1.5f; light[3].position[3] = 1.0f;
\
	//light[3].position[0] = iron[0]; light[3].position[1] = iron[1]; // spot light position in WC
	//light[3].position[2] = iron[2]; light[3].position[3] = iron[3];

	light[3].ambient_color[0] = 1.0f; light[3].ambient_color[1] = 1.0f;
	light[3].ambient_color[2] = 0.0f; light[3].ambient_color[3] = 1.0f;

	light[3].diffuse_color[0] = 1.0f; light[3].diffuse_color[1] = 1.0f;
	light[3].diffuse_color[2] = 0.0f; light[3].diffuse_color[3] = 1.0f;

	light[3].specular_color[0] = 1.0f; light[3].specular_color[1] = 1.0f;
	light[3].specular_color[2] = 0.0f; light[3].specular_color[3] = 1.0f;

	light[3].spot_direction[0] = 0.0f; light[3].spot_direction[1] = 1.0f; // spot light direction in WC
	light[3].spot_direction[2] = 0.0f;
	light[3].spot_cutoff_angle = 10.0f;
	light[3].spot_exponent =5.0f;


	//0번 광원
	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glUniform4fv(loc_light[0].position, 1, light[0].position);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);

	//1번광원
	glUniform1i(loc_light[1].light_on, light[1].light_on);
	// need to supply position in EC for shading
	glm::vec4 position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1],
		light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);
	// glUseProgram(0);


	//2번 광원
	glUniform1i(loc_light[2].light_on, light[2].light_on);
	// need to supply position in EC for shading
	position_EC = ViewMatrix * glm::vec4(light[2].position[0], light[2].position[1], light[2].position[2], light[2].position[3]);
	glUniform4fv(loc_light[2].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light[2].specular_color, 1, light[2].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[2].spot_direction[0], light[2].spot_direction[1], light[2].spot_direction[2]);
	glUniform3fv(loc_light[2].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light[2].spot_exponent, light[2].spot_exponent);

	/**/
	//3번 광원
	glUniform1i(loc_light[3].light_on, light[3].light_on);
	// need to supply position in EC for shading
	position_EC = ViewMatrix* glm::vec4(light[3].position[0], light[3].position[1], light[3].position[2], light[3].position[3]);
	glUniform4fv(loc_light[3].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[3].ambient_color, 1, light[3].ambient_color);
	glUniform4fv(loc_light[3].diffuse_color, 1, light[3].diffuse_color);
	glUniform4fv(loc_light[3].specular_color, 1, light[3].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	direction_EC =glm::mat3(ViewMatrix) * glm::vec3(light[3].spot_direction[0], light[3].spot_direction[1], light[3].spot_direction[2]);
	glUniform3fv(loc_light[3].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
	glUniform1f(loc_light[3].spot_exponent, light[3].spot_exponent);



	/*
	//3번 광원
	glUniform1i(loc_light[3].light_on, light[0].light_on);
	glUniform4fv(loc_light[3].position, 1, light[0].position);
	glUniform4fv(loc_light[3].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[3].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[3].specular_color, 1, light[0].specular_color);
	*/
	glUseProgram(0);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_floor();
	prepare_tiger();
	prepare_ben();
	prepare_wolf();
	prepare_spider();
	prepare_dragon();
	prepare_optimus();
	prepare_cow();
	prepare_bus();
	prepare_bike();
	prepare_godzilla();
	prepare_ironman();
	prepare_tank();
	set_up_scene_lights();
	initialize_screen();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 3D Objects";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: '0', '1', 'a', 't', 'f', 'c', 'd', 'y', 'u', 'i', 'o', 'ESC'" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}