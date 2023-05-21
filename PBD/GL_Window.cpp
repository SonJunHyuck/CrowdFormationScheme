#include "GL_Window.h"

GL_Window::GL_Window()
{

}

GL_Window::GL_Window(int w, int h)
{
	set_size(w, h);

	sphere = new Sphere(0.4, 10, 10);
	sphere_formation = new Sphere(50.0f, 100, 100);
	checkerd_board = new CheckerdBoard(600, 8);

	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);
	viewer->zoom(-1.5f);
}

void GL_Window::set_size(int w, int h)
{
	width = w;
	height = h;
}

void GL_Window::setup_buffer()
{
	shader_cube = new ShaderProgram();
	shader_cube->initFromFiles("ColorCube.vert", "ColorCube.frag");

	shader_cube->addUniform("ViewMatrix");  //View*Model : mat4
	shader_cube->addUniform("ModelViewMatrix");  //View*Model : mat4
	shader_cube->addUniform("NormalMatrix"); //Refer next slide : mat3
	shader_cube->addUniform("MVP");

	shader_cube->addUniform("LightPosition");
	shader_cube->addUniform("LightIntensity");
	shader_cube->addUniform("Ka");  //ambient Light Color : vec3
	shader_cube->addUniform("Kd");  //Diffuse Object Color :vec3
	shader_cube->addUniform("Ks");  //Specular Object Color :vec3

	shader_cube->addUniform("CamPos");  //Specular Object Color :vec3
	shader_cube->addUniform("Shiness");

	shader_sphere = new ShaderProgram();
	shader_sphere->initFromFiles("Sphere.vert", "Sphere.frag");

	shader_sphere->addUniform("viewMatrix");  //View*Model : mat4
	shader_sphere->addUniform("projectionMatrix");  //View*Model : mat4

	shader_checkerd_board = new ShaderProgram();
	shader_checkerd_board->initFromFiles("CheckeredBoard.vert", "CheckeredBoard.frag");

	shader_checkerd_board->addUniform("ViewMatrix");  //View*Model : mat4
	shader_checkerd_board->addUniform("ModelViewMatrix");  //View*Model : mat4
	shader_checkerd_board->addUniform("NormalMatrix"); //Refer next slide : mat3
	shader_checkerd_board->addUniform("MVP");

	shader_checkerd_board->addUniform("LightPosition");
	shader_checkerd_board->addUniform("LightIntensity");
	shader_checkerd_board->addUniform("Ka");  //ambient Light Color : vec3
	shader_checkerd_board->addUniform("Kd");  //Diffuse Object Color :vec3
	shader_checkerd_board->addUniform("Ks");  //Specular Object Color :vec3

	shader_checkerd_board->addUniform("CamPos");  //Specular Object Color :vec3
	shader_checkerd_board->addUniform("Shiness");

	shader_mesh = new ShaderProgram();
	shader_mesh->initFromFiles("ColorCube.vert", "ColorCube.frag");

	shader_mesh->addUniform("ViewMatrix");  //View*Model : mat4
	shader_mesh->addUniform("ModelViewMatrix");  //View*Model : mat4
	shader_mesh->addUniform("NormalMatrix"); //Refer next slide : mat3
	shader_mesh->addUniform("MVP");

	shader_mesh->addUniform("LightPosition");
	shader_mesh->addUniform("LightIntensity");
	shader_mesh->addUniform("Ka");  //ambient Light Color : vec3
	shader_mesh->addUniform("Kd");  //Diffuse Object Color :vec3
	shader_mesh->addUniform("Ks");  //Specular Object Color :vec3

	shader_mesh->addUniform("CamPos");  //Specular Object Color :vec3
	shader_mesh->addUniform("Shiness");

	mesh = new Mesh("Bird.obj", shader_mesh);
}

void GL_Window::setup_instance_buffer(int num_particles)
{
	mat_sphere_instancing = new glm::mat4[num_particles];
	
	glGenBuffers(1, &VBO_mat_sphere_instancing);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_mat_sphere_instancing);
	glBufferData(GL_ARRAY_BUFFER, num_particles * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

	glBindVertexArray(sphere->VAO);
	// vertex attributes
	std::size_t vec4Size = sizeof(glm::vec4);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	ka_instancing = new glm::vec4[num_particles];

	glGenBuffers(1, &VBO_ka_instancing);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ka_instancing);
	glBufferData(GL_ARRAY_BUFFER, num_particles * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, vec4Size, (void*)0);
	glVertexAttribDivisor(7, 1);
}

void GL_Window::set_formation(Simulation* sim)
{
	int i = 0;

	// Blue
	float density = DENSITY;
	float init_x = 60.0f; // RIGHT_BOUND_X - 300.0f;
	float init_z = 0.0f;
	float mass = 1.0f;
	float radius = 0.4f;
	int group_id = 0;

	glm::vec3 velocity = VEC_ZERO;
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 pos_init = glm::vec3(init_x, 0, init_z);
	glm::vec3 pos_offset = glm::vec3(-200, 0, 0);
	glm::vec3 pos_goal = pos_init + pos_offset;

	Group* group = new Group(group_id, pos_init, pos_goal);
	sim->groups[0] = group;
	sim->planner->map_groups.insert({ 0, group });

	shape_type type = shape_type::MESH;
	switch (type)
	{
	case shape_type::SQUARE:
		for (int i_ = 0; i_ < ROWS / 2; i_++)
		{
			for (int j_ = 0; j_ < COLS; j_++)
			{
				float interval_weight = 0.6f;  // 간격 
				float x = init_x + j_ * density * interval_weight + rand_interval(-0.4, 0.4);  // 좌하단에서 우상단으로
				float z = init_z - i_ * density * interval_weight + rand_interval(-0.4, 0.4) - HEIGHT_DIFF;
				glm::vec3 pos = glm::vec3(x, 0, z);
				glm::vec3 goal = pos + pos_offset;

				Particle* particle = new Particle(pos, velocity, mass, radius, group_id, color, goal);

				sim->particles[i] = particle;
				i++;
			}
		}
		break;
	case shape_type::CIRCLE:
	{
		float D2R = _M_PI / 180.0f;
		float r_weight = 0;

		for (int i_ = 0; i_ < sim->num_particles_half;)
		{
			int angle = r_weight == 0 ? 360 : (120 / r_weight);

			for (int j_ = 0; j_ < 360; j_ += angle)
			{
				float r = r_weight;
				float x = init_x + r * cos(j_ * D2R);
				float z = init_z + r * sin(j_ * D2R) - 20.0f;

				glm::vec3 pos = glm::vec3(x, 0, z);
				glm::vec3 goal = pos + pos_offset;

				/*if (i == 0)
				{
					color = glm::vec3(1, 1, 1);
				}
				else
				{
					color = glm::vec3(0, 0, 1);
				}*/

				Particle* particle = new Particle(pos, velocity, mass, radius, group_id, color, goal);

				sim->particles[i] = particle;
				i++;
				i_++;

				if (i_ >= sim->num_particles_half)
					break;
			}
			r_weight += density;// *0.8f;
		}
	}
	break;
	case shape_type::MESH:
		std::vector<glm::vec3> formation = set_mesh(sim->num_particles_half, 0, 1, 0, 2.0f);

		// create particle
		for (int i_ = 0; i_ < sim->num_particles_half;)
		{
			float x = init_x + 0 + formation[i_].x;
			float z = init_z + 25 + formation[i_].z + 40.3f;
			glm::vec3 pos = glm::vec3(x, 0, z);
			glm::vec3 goal = pos + pos_offset;

			Particle* particle = new Particle(pos, velocity, mass, radius, group_id, color, goal);

			sim->particles[i] = particle;
			i++;
			i_++;
		}
		break;
	}

	if (!IS_SINGLE)
	{
		// Red
		density = DENSITY;
		init_x = -60.0f; //LEFT_BOUND_X + 200.0f;
		init_z = 0.0f;
		mass = 1.0f;
		radius = 0.4f;
		group_id = 1;

		velocity = VEC_ZERO;
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		pos_init = glm::vec3(init_x, 0, init_z);
		pos_offset = glm::vec3(200, 0, 0);
		pos_goal = pos_init + pos_offset;

		Group* group = new Group(group_id, pos_init, pos_goal);
		sim->groups[1] = group;
		sim->planner->map_groups.insert({ 1, group });

		type = shape_type::MESH;
		switch (type)
		{
		case shape_type::SQUARE:
			for (int i_ = 0; i_ < ROWS / 2; i_++)
			{
				for (int j_ = 0; j_ < COLS; j_++)
				{
					float height_weight = 0;
					height_weight = i_ > ROWS / 4 ? -MIDDLE_HOLE : MIDDLE_HOLE;

					float interval_weight = 0.6f;  // 간격 조절	
					float x = init_x - j_ * density * interval_weight + rand_interval(-0.4, 0.4);  // 우하단에서 좌상단으로
					float z = init_z - i_ * density * interval_weight + rand_interval(-0.4, 0.4) + height_weight;
					glm::vec3 pos = glm::vec3(x, 0, z);
					glm::vec3 goal = pos + pos_offset;

					/*if (i == 865)
					{
						color = glm::vec3(1, 1, 1);
					}
					else
					{
						color = glm::vec3(1, 0, 0);
					}*/
					Particle* particle = new Particle(pos, velocity, mass, radius, group_id, color, goal);

					sim->particles[i] = particle;
					i++;
				}
			}
			break;
		case shape_type::CIRCLE:
		{
			float D2R = _M_PI / 180.0f;
			float r_weight = 0;

			for (int i_ = 0; i_ < sim->num_particles_half;)
			{
				int angle = r_weight == 0 ? 360 : (120 / r_weight);

				for (int j_ = 0; j_ < 360; j_ += angle)
				{
					float r = r_weight;
					float x = init_x + r * cos(j_ * D2R);
					float z = init_z + r * sin(j_ * D2R);

					glm::vec3 pos = glm::vec3(x, 0, z);
					glm::vec3 goal = pos + pos_offset;

					Particle* particle = new Particle(pos, velocity, mass, radius, group_id, color, goal);

					sim->particles[i] = particle;
					i++;
					i_++;

					if (i_ >= sim->num_particles_half)
						break;
				}
				r_weight += density * 0.6f;
			}
		}
		break;
		case shape_type::MESH:
			std::vector<glm::vec3> formation = set_mesh(sim->num_particles_half, 0, 1, 0, 2.0f);

			// create particle
			for (int i_ = 0; i_ < sim->num_particles_half;)
			{
				float x = init_x + 0 - formation[i_].x;
				float z = init_z + 25 - formation[i_].z - 40.0f;
				glm::vec3 pos = glm::vec3(x, 0, z);
				glm::vec3 goal = pos + pos_offset;

				Particle* particle = new Particle(pos, velocity, mass, radius, group_id, color, goal);

				sim->particles[i] = particle;
				i++;
				i_++;
			}
			break;
		}
	}
}

void GL_Window::change_particle_color(Simulation* sim)
{
	// VBO_mat_sphere_instancing에 작업을 할 것이다.
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ka_instancing);

	// VBO를 가리키는 포인터를 획득
	void* pointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	// 포인터에 해당하는 곳에 실질적 데이터를 채워줌 (pointer, data, size)
	memcpy(pointer, &ka_instancing[0], sim->num_particles * sizeof(glm::vec4));
	// 버퍼 채우기 종료
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void GL_Window::draw(Simulation* sim)
{
	// background color
	glClearColor(0.2f, 0.2f, 0.2f, 0);

	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 오브젝트가 겹치는 경우 뒤에 있는 친구가 보이지 않게 하기 위함
	glEnable(GL_DEPTH_TEST);

	// 화면 4등분 후 우측 상단에 앵커 포인트
	glViewport(0, 0, width, height);

#pragma region MVP Matrix
	glm::mat4 identity = glm::identity<glm::mat4>();

	glm::mat4 translate = glm::translate(identity, VEC_ZERO);
	glm::mat4 rotate = glm::translate(identity, VEC_ZERO);
	glm::mat4 scale = glm::scale(identity, VEC_ONE);

	glm::vec3 eye = viewer->getViewPoint();
	glm::vec3 look = viewer->getViewCenter();
	glm::vec3 up = viewer->getUpVector();

	glm::mat4 model = translate * rotate * scale;  // model	
	glm::mat4 view = LookAt(eye, look, up);  // view
	glm::mat4 projection = Perspective(45.0f, width / height, 0.1f, 500.0f);  //projection matrix


	glm::mat4 mvp = projection * view * model;
	glm::mat4 modelViewMatrix = view * model;
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));

#pragma endregion

#pragma region Lighting
	glm::vec4 lightLocation = glm::vec4(glm::vec3(10, 10, 10), 1);
	glm::vec3 lightIntensity = glm::vec3(0.9f, 0.9f, 0.9f);
	glm::vec3 ka = glm::vec3(0.1745, 0.01175, 0.01175);
	glm::vec3 kd = glm::vec3(0.61424, 0.04136, 0.04136);
	glm::vec3 ks = glm::vec3(0.727811, 0.626959, 0.626959);
	glm::vec3 camPos = glm::vec3(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	float shiness = 76.8f;
#pragma endregion

#pragma region CheckerBoard
	shader_checkerd_board->use();

	glUniformMatrix4fv(shader_checkerd_board->uniform("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(shader_checkerd_board->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniformMatrix3fv(shader_checkerd_board->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(shader_checkerd_board->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

	glUniform3fv(shader_checkerd_board->uniform("Ka"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
	glUniform3fv(shader_checkerd_board->uniform("Kd"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
	glUniform3fv(shader_checkerd_board->uniform("Ks"), 1, glm::value_ptr(VEC_ZERO));
	glUniform4fv(shader_checkerd_board->uniform("LightPosition"), 1, glm::value_ptr(lightLocation));
	glUniform3fv(shader_checkerd_board->uniform("LightIntensity"), 1, glm::value_ptr(lightIntensity));
	glUniform3fv(shader_checkerd_board->uniform("CamPos"), 1, glm::value_ptr(camPos));
	glUniform1f(shader_checkerd_board->uniform("Shiness"), shiness);

	if (checkerd_board)
		checkerd_board->draw();

	shader_checkerd_board->disable();
#pragma endregion

#pragma region Particle

	for (int i = 0; i < sim->num_particles; i++)
	{
		float angle = deg_to_rad(acosf(dot(glm::normalize(sim->particles[i]->V), glm::vec3(1, 0, 0))));

		// model matrix
		mat_sphere_instancing[i] = glm::translate(identity, sim->particles[i]->X) * glm::rotate(identity, angle, VEC_UP);

		ka_instancing[i] = glm::vec4(sim->particles[i]->color, 1);
	}

	// VBO_mat_sphere_instancing에 작업을 할 것이다.
	glBindBuffer(GL_ARRAY_BUFFER, VBO_mat_sphere_instancing);

	// VBO를 가리키는 포인터를 획득
	void* pointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	// 포인터에 해당하는 곳에 실질적 데이터를 채워줌 (pointer, data, size)
	memcpy(pointer, &mat_sphere_instancing[0], sim->num_particles * sizeof(glm::mat4));

	// 버퍼 채우기 종료
	glUnmapBuffer(GL_ARRAY_BUFFER);

	change_particle_color(sim);

	shader_sphere->use();


	glUniformMatrix4fv(shader_sphere->uniform("viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(shader_sphere->uniform("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));

	if (sphere)
	{
		glBindVertexArray(sphere->VAO);

		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0, sim->num_particles);

		glEnable(GL_CULL_FACE);  // Cull face을 켬
		glCullFace(GL_BACK); // 오브젝트의 back 부분을 Culling

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	shader_sphere->disable();
#pragma endregion

#pragma region SHORT_RANGE_DESTINATION
	/*
	{
		for (int i = 0; i < sim->num_particles; i++)
		{
			// model matrix
			mat_sphere_instancing[i] = glm::translate(identity, sim->particles[i]->offset);
			ka_instancing[i] = glm::vec4(glm::vec3(1, 1, 0), 1);
		}

		// VBO_mat_sphere_instancing에 작업을 할 것이다.
		glBindBuffer(GL_ARRAY_BUFFER, VBO_mat_sphere_instancing);

		// VBO를 가리키는 포인터를 획득
		pointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		// 포인터에 해당하는 곳에 실질적 데이터를 채워줌 (pointer, data, size)
		memcpy(pointer, &mat_sphere_instancing[0], sim->num_particles * sizeof(glm::mat4));

		// 버퍼 채우기 종료
		glUnmapBuffer(GL_ARRAY_BUFFER);

		change_particle_color(sim);

		shader_sphere->use();

		glUniformMatrix4fv(shader_sphere->uniform("viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(shader_sphere->uniform("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));

		if (sphere)
		{
			glBindVertexArray(sphere->VAO);

			int size;
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0, sim->num_particles);

			glEnable(GL_CULL_FACE);  // Cull face을 켬
			glCullFace(GL_BACK); // 오브젝트의 back 부분을 Culling

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		shader_sphere->disable();
	}*/

	/*
	for (int j = 0; j < sim->num_groups; j++)
	{
		glm::vec3 line_color = glm::vec3(1, 1, 0);

		for (int i = 0; i < sim->num_particles; i++)
		{
			glm::mat4 model = identity;
			line = new Line(sim->particles[i]->X, sim->particles[i]->offset);
			model = glm::translate(identity, VEC_ZERO);

			glm::mat4 mvp = projection * view * model;
			glm::mat4 model_View_Matrix = view * model;
			glm::mat3 normal_Matrix = glm::mat3(glm::transpose(glm::inverse(model_View_Matrix)));

			shader_cube->use();

			glUniformMatrix4fv(shader_cube->uniform("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(shader_cube->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(model_View_Matrix));
			glUniformMatrix3fv(shader_cube->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_Matrix));
			glUniformMatrix4fv(shader_cube->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

			glUniform3fv(shader_cube->uniform("LightIntensity"), 1, glm::value_ptr(lightIntensity));
			glUniform4fv(shader_cube->uniform("LightPosition"), 1, glm::value_ptr(lightLocation));

			glUniform3fv(shader_cube->uniform("Ka"), 1, glm::value_ptr(line_color));
			glUniform3fv(shader_cube->uniform("Kd"), 1, glm::value_ptr(kd));
			glUniform3fv(shader_cube->uniform("Ks"), 1, glm::value_ptr(ks));

			glUniform3fv(shader_cube->uniform("CamPos"), 1, glm::value_ptr(camPos));
			glUniform1f(shader_cube->uniform("Shiness"), shiness);

			if (line)
				line->draw();

			shader_cube->disable();
		}
	}*/
#pragma endregion

#pragma region STATION
	// Point
	/*{
		for (int j = 0; j < sim->num_groups; j++)
		{
			for (int i = 0; i < sim->groups[j]->path_counter - 1; i++)
			{
				// model matrix
				mat_sphere_instancing[i] = glm::translate(identity, sim->groups[j]->path[i].pos);
				ka_instancing[i] = glm::vec4(glm::vec3(0, 1, j), 1);
			}

			// VBO_mat_sphere_instancing에 작업을 할 것이다.
			glBindBuffer(GL_ARRAY_BUFFER, VBO_mat_sphere_instancing);

			// VBO를 가리키는 포인터를 획득
			pointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			// 포인터에 해당하는 곳에 실질적 데이터를 채워줌 (pointer, data, size)
			memcpy(pointer, &mat_sphere_instancing[0], sim->groups[j]->path_counter * sizeof(glm::mat4));

			// 버퍼 채우기 종료
			glUnmapBuffer(GL_ARRAY_BUFFER);

			change_particle_color(sim);

			shader_sphere->use();

			glUniformMatrix4fv(shader_sphere->uniform("viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(shader_sphere->uniform("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));

			if (sphere)
			{
				glBindVertexArray(sphere->VAO);

				int size;
				glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
				glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0, sim->groups[j]->path_counter);

				glEnable(GL_CULL_FACE);  // Cull face을 켬
				glCullFace(GL_BACK); // 오브젝트의 back 부분을 Culling

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}

			shader_sphere->disable();

		}
	}*/

	// Line
	 /*
	for (int j = 0; j < sim->num_groups; j++)
	{
		glm::vec3 line_color = glm::vec3(j ,0, 1 - j);

		for (int i = 0; i < sim->groups[j]->path_counter - 1; i++)
		{
			glm::mat4 model = identity;
			line = new Line(sim->groups[j]->path[i].pos, sim->groups[j]->path[i + 1].pos);
			model = glm::translate(identity, VEC_ZERO);

			glm::mat4 mvp = projection * view * model;
			glm::mat4 model_View_Matrix = view * model;
			glm::mat3 normal_Matrix = glm::mat3(glm::transpose(glm::inverse(model_View_Matrix)));

			shader_cube->use();

			glUniformMatrix4fv(shader_cube->uniform("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(shader_cube->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(model_View_Matrix));
			glUniformMatrix3fv(shader_cube->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_Matrix));
			glUniformMatrix4fv(shader_cube->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

			glUniform3fv(shader_cube->uniform("LightIntensity"), 1, glm::value_ptr(lightIntensity));
			glUniform4fv(shader_cube->uniform("LightPosition"), 1, glm::value_ptr(lightLocation));

			glUniform3fv(shader_cube->uniform("Ka"), 1, glm::value_ptr(line_color));
			glUniform3fv(shader_cube->uniform("Kd"), 1, glm::value_ptr(kd));
			glUniform3fv(shader_cube->uniform("Ks"), 1, glm::value_ptr(ks));

			glUniform3fv(shader_cube->uniform("CamPos"), 1, glm::value_ptr(camPos));
			glUniform1f(shader_cube->uniform("Shiness"), shiness);

			if (line)
				line->draw();

			shader_cube->disable();
		}
	}
	*/
#pragma endregion

#pragma region Wall

	ka = glm::vec3(0.081745, 0.081175, 0.07175);
	kd = glm::vec3(0.081424, 0.084136, 0.07136);
	ks = glm::vec3(0.0627811, 0.0826959, 0.0726959);

	for (int i = 0; i < sim->num_walls; i++)
	{
		float width = sim->walls[i]->width; // glm::abs(sim->walls[i]->t.x) + (1.0f + sim->walls[i]->margin * 0.3f) * 1.05;
		float height = 3.0f;
		float length = sim->walls[i]->height; // glm::abs(sim->walls[i]->t.z) + (1.0f + sim->walls[i]->margin * 0.3f) * 1.05;
		cube = new ColorCube(width, height, length);

		glm::mat4 model = identity;
		model = glm::translate(identity, (glm::vec3((sim->walls[i]->x0.x + sim->walls[i]->x1.x) / 2, 0, (sim->walls[i]->x0.z + sim->walls[i]->x1.z) / 2))) * rotate * scale;

		glm::mat4 mvp = projection * view * model;
		glm::mat4 model_View_Matrix = view * model;
		glm::mat3 normal_Matrix = glm::mat3(glm::transpose(glm::inverse(model_View_Matrix)));

		shader_cube->use();

		glUniformMatrix4fv(shader_cube->uniform("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(shader_cube->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(model_View_Matrix));
		glUniformMatrix3fv(shader_cube->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_Matrix));
		glUniformMatrix4fv(shader_cube->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

		glUniform3fv(shader_cube->uniform("LightIntensity"), 1, glm::value_ptr(lightIntensity));
		glUniform4fv(shader_cube->uniform("LightPosition"), 1, glm::value_ptr(lightLocation));

		glUniform3fv(shader_cube->uniform("Ka"), 1, glm::value_ptr(ka));
		glUniform3fv(shader_cube->uniform("Kd"), 1, glm::value_ptr(kd));
		glUniform3fv(shader_cube->uniform("Ks"), 1, glm::value_ptr(ks));

		glUniform3fv(shader_cube->uniform("CamPos"), 1, glm::value_ptr(camPos));
		glUniform1f(shader_cube->uniform("Shiness"), shiness);

		if (cube)
			cube->draw();

		shader_cube->disable();
	}


#pragma endregion

#pragma region Mesh

	/*ka = glm::vec3(0.081745, 0.081175, 0.07175);
	kd = glm::vec3(0.081424, 0.084136, 0.07136);
	ks = glm::vec3(0.0627811, 0.0826959, 0.0726959);

	glm::mat4 model = identity;
	rotate = glm::rotate(identity, deg_to_rad(00.0f), glm::vec3(0, 1, 0));
	scale = glm::scale(identity, glm::vec3(1.5f, 1.5f, 1.5f));
	model = glm::translate(identity, glm::vec3(70.0f, 1.0f, -70.0f)) * rotate * scale;

	glm::mat4 mvp = projection * view * model;
	glm::mat4 model_View_Matrix = view * model;
	glm::mat3 normal_Matrix = glm::mat3(glm::transpose(glm::inverse(model_View_Matrix)));

	shader_mesh->use();

	glUniformMatrix4fv(shader_mesh->uniform("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(shader_mesh->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(model_View_Matrix));
	glUniformMatrix3fv(shader_mesh->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_Matrix));
	glUniformMatrix4fv(shader_mesh->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

	glUniform3fv(shader_mesh->uniform("LightIntensity"), 1, glm::value_ptr(lightIntensity));
	glUniform4fv(shader_mesh->uniform("LightPosition"), 1, glm::value_ptr(lightLocation));

	glUniform3fv(shader_mesh->uniform("Ka"), 1, glm::value_ptr(ka));
	glUniform3fv(shader_mesh->uniform("Kd"), 1, glm::value_ptr(kd));
	glUniform3fv(shader_mesh->uniform("Ks"), 1, glm::value_ptr(ks));

	glUniform3fv(shader_mesh->uniform("CamPos"), 1, glm::value_ptr(camPos));
	glUniform1f(shader_mesh->uniform("Shiness"), shiness);

	if (mesh)
		mesh->draw();

	shader_mesh->disable();*/

#pragma endregion

	if (CAMERA_TRACKING)
	{
		// 임시 카메라 트래킹
		if (!sim->stop)
		{
			tracking_particle = 300; //713; //300;   // 빨간색 첫열 가운데
			float fractionChangeX = static_cast<float>(sim->particles[tracking_particle]->X.x - tracking_particle_last_pos.x) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(tracking_particle_last_pos.z - sim->particles[tracking_particle]->X.z) / static_cast<float>(height);

			tracking_particle_last_pos = sim->particles[tracking_particle]->X;

			viewer->centerAt(sim->particles[tracking_particle]->X);
			viewer->translate(-fractionChangeX, -fractionChangeY, 1);
		}
	}
}

void GL_Window::dummy_init(Simulation* sim)
{
	srand(time(NULL));

	// Shader Data Setup
	setup_instance_buffer(sim->num_particles);
	setup_buffer();

#pragma region INIT_PARTICLE
	set_formation(sim);
#pragma endregion

#pragma region INIT_GROUP
	for (int i = 0; i < sim->num_groups; i++)
	{
		Group* g = sim->groups[i];
		
		int particle_start_i = sim->num_particles_half * i;
		int particle_end_i = sim->num_particles_half * (i + 1);

		for (int j = particle_start_i; j < particle_end_i; j++)
		{
			Particle* p = sim->particles[j];

			if (g->id == p->group_id)
				g->map_particles.insert({ j, p });
		}

		g->init();
		g->set_center();
	}
#pragma endregion

#pragma region INIT_CONSTRAINT
	std::vector<particle_tuple*> friction_pairs = get_tuples(sim->num_particles);
	int trig_len = 1 + (sim->num_particles * (sim->num_particles + 1) / 2);

	sim->stability_upper_trig_arr =
		(Constraint**)malloc(sizeof(void*) * trig_len);
	sim->collision_upper_trig_arr =
		(Constraint**)malloc(sizeof(void*) * trig_len);
	sim->powerlaw_upper_trig_arr =
		(Constraint**)malloc(sizeof(void*) * trig_len);
	sim->distance_trig_arr =
		(Constraint**)malloc(sizeof(void*) * trig_len);

	for (std::vector<particle_tuple*>::iterator it = friction_pairs.begin();
		it != friction_pairs.end(); ++it)
	{
		Stability_Constraint* stab = new Stability_Constraint(sim, (*it)->i, (*it)->j);
		Friction_Constraint* fc = new Friction_Constraint(sim, (*it)->i, (*it)->j);
		Powerlaw_Constraint* pl = new Powerlaw_Constraint(sim, (*it)->i, (*it)->j);
		Distance_Constraint* ds = new Distance_Constraint(sim, (*it)->i, (*it)->j);

		// 해당 tuple 전에 관계는 다 적립된 상태
		if ((*it)->i < (*it)->j)
		{
			sim->collision_map[(*it)->i * sim->num_particles + (*it)->j] = fc;
			int r = (*it)->i;
			int c = (*it)->j;
			int t_idx = (sim->num_particles * r) + c - (r * (r + 1) * 0.5);
			sim->collision_upper_trig_arr[t_idx] = fc;
			sim->powerlaw_upper_trig_arr[t_idx] = pl;
			sim->stability_upper_trig_arr[t_idx] = stab;
			sim->distance_trig_arr[t_idx] = ds;
		}
	}
#pragma endregion

#pragma region INIT_WALL
	sim->num_walls = 0;
	sim->walls = (Wall**)malloc(sizeof(void*) * sim->num_walls);
	/*sim->walls[0] =
		new Wall(glm::vec3(-170, 0, GROUND_HEIGHT - 20),
			glm::vec3(150., 0, GROUND_HEIGHT - 10), glm::vec3(0., 0, 1.));
	sim->walls[1] =
		new Wall(glm::vec3(-170, 0, GROUND_HEIGHT - 110),
			glm::vec3(150., 0, GROUND_HEIGHT - 110), glm::vec3(0., 0, -1.));*/
	//sim->walls[2] =
	//	new Wall(glm::vec2(-10, GROUND_HEIGHT - 110),
	//		glm::vec2(10, GROUND_HEIGHT - 50), glm::vec2(0., -1.));
	//sim->walls[3] =
	//	new Wall(glm::vec2(-10, GROUND_HEIGHT - 45),
	//		glm::vec2(10, GROUND_HEIGHT - 10), glm::vec2(0., -1.));

	//sim->num_constraints = sim->num_particles *2 + sim->num_particles * sim->num_walls; // ground + walls + mesh
	sim->num_constraints = sim->num_particles;
	sim->constraints = (Constraint**)malloc(sizeof(void*) * sim->num_constraints);
	int constraint_ctr = 0;

	//// ground
	//for (int i = 0; i < sim->num_particles; i++)
	//{
	//	sim->constraints[i] = new Ground_Constraint(sim, i);
	//	constraint_ctr++;
	//}

	//// wall
	//for (int i = 0; i < sim->num_particles; i++)
	//{
	//	for (int j = 0; j < sim->num_walls; j++)
	//	{
	//		sim->constraints[constraint_ctr] = new Wall_Constraint(sim, i, j);
	//		constraint_ctr++;
	//	}
	//}

	// mesh
	for (int i = 0; i < sim->num_particles; i++)
	{
		sim->constraints[constraint_ctr] = new Mesh_Constraint(sim, i);
		constraint_ctr++;
	}

#pragma endregion

#pragma region INIT_STATION
	//{
	//	int station_id = 0;
	//	Station station;
	//	PathPlanner* planner = sim->planner;

	//	// 1. Start Station, End Station ref Group
	//	for (auto group : planner->map_groups)
	//	{
	//		station = Station(station_id, group.second->start);
	//		sim->stations.push_back(station);
	//		group.second->station_start_id = station_id;
	//		station_id++;

	//		station = Station(station_id, group.second->end);
	//		sim->stations.push_back(station);
	//		group.second->station_end_id = station_id;
	//		station_id++;
	//	}

	//	// 2. Wall Stations (모든 벽은 4개의 vertices을 가지고 있다.)
	//	for (int i = 0; i < sim->num_walls; i++)
	//	{
	//		// 4개의 vertices
	//		for (int j = 0; j < 4; j++)
	//		{
	//			station = Station(station_id, sim->walls[i]->vertices[j]);
	//			sim->stations.push_back(station);
	//			station_id++;
	//		}
	//	}

	//	// 3-1. Random Stations
	//	/*
	//	{
	//	for (int i = 0; i < STATION_RANDOM_COUNT; i++)
	//	{
	//		int shrink_x = 200;
	//		int shrink_y = 200;
	//		glm::vec2 pos_random = rand_glm::vec2(
	//			GRID_MIN_X + shrink_x, GRID_MAX_X - shrink_x,
	//			GRID_MIN_Y + shrink_y * 2, GRID_MAX_Y - shrink_y * 0.5f);

	//		// 장애물과 겹치면 다시 생성
	//		for (int j = 0; j < sim->num_walls; j++)
	//		{
	//			if (sim->walls[j]->is_intersect_point(pos_random))
	//			{
	//				j--;
	//				pos_random = rand_glm::vec2(
	//					GRID_MIN_X + shrink_x, GRID_MAX_X - shrink_x,
	//					GRID_MIN_Y + shrink_y * 2, GRID_MAX_Y - shrink_y * 0.5f);
	//			}
	//		}

	//		station = Station(station_id, pos_random);
	//		sim->stations.push_back(station);
	//		station_id++;
	//	}
	//	}*/

	//	// 3-2. Grid 형태로 Station 생성
	//	auto list = sim->grid->insert_station();
	//	for (auto i : list)
	//	{
	//		bool is_intersect = false;
	//		for (int j = 0; j < sim->num_walls; j++)
	//		{
	//			if (sim->walls[j]->is_intersect_point(i))
	//			{
	//				is_intersect = true;
	//				break;
	//			}
	//		}

	//		if (!is_intersect)
	//		{
	//			station = Station(station_id, i);
	//			sim->stations.push_back(station);
	//			station_id++;
	//		}
	//	}

	//	// 4. Link each Station
	//	for (int i = 0; i < sim->stations.size(); i++)
	//	{
	//		sim->stations[i].search(sim->stations, sim->walls, sim->num_walls);
	//	}

	//	// 5. a_star
	//	for (auto group : planner->map_groups)
	//	{
	//		std::vector<int> path = a_star(sim->stations, group.second->station_start_id, group.second->station_end_id);

	//		for (int j = 0; j < path.size(); j++)
	//		{
	//			int station_index = path[j];
	//			station = sim->stations[station_index];
	//			group.second->path.push_back(station);
	//		}
	//	}

	//	// 6. A* 적용, 각 agent가 path를 가짐
	//	for (auto group : planner->map_groups)
	//	{
	//		group.second->init_particles_goal();
	//	}
	//}
#pragma endregion

#pragma region INIT_PARTICLE_VELOCITY
	for (int i = 0; i < sim->num_particles; i++)
	{
		sim->particles[i]->V_pref = V_PREF_ACCEL;

		float u;
		do 
		{
			u = (float)rand() / (float)RAND_MAX;
		} while (u >= 1.0);

		sim->particles[i]->V_pref +=
			sqrtf(-2.f * logf(1.f - u)) * 0.1f *
			cosf(2.f * _M_PI * (float)rand() / (float)RAND_MAX);

		sim->planner->calc_pref_v_force(i);
		sim->particles[i]->V.x = sim->planner->velocity_buffer[i].x;
		sim->particles[i]->V.y = sim->planner->velocity_buffer[i].y;
	}
#pragma endregion

#pragma region SHADER_INSTANCING

	for (int i = 0; i < sim->num_particles; i++)
	{
		ka_instancing[i] = glm::vec4(sim->particles[i]->color, 1);
	}

	change_particle_color(sim);
#pragma endregion
}

void GL_Window::create_station(glm::vec3 pos, Simulation* sim, int group_id)
{
	// station에 인덱싱을 부여하고 인덱싱을 따라가기

	// 마우스 위치를 받아서, 클릭했을 때, 3차원 좌표를 printf
	printv(pos);

	// 정상 포지션이면 station을 생성
	Station st = Station(sim->groups[group_id]->path_counter++, pos);
	sim->groups[group_id]->path.push_back(st);
	sim->groups[group_id]->update_start_end();
}

void GL_Window::send_ray(glm::vec4 ray_clip, Simulation* sim, int group_id)
{
	glm::vec3 eye = viewer->getViewPoint();
	glm::vec3 look = viewer->getViewCenter();
	glm::vec3 up = viewer->getUpVector();

	glm::mat4 view = LookAt(eye, look, up);  // view
	glm::mat4 projection = Perspective(45.0f, width / height, 0.1f, 500.0f);  //projection matrix

	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0);
	
	glm::vec3 ray_world = (glm::vec3)(glm::inverse(view) * ray_eye);
	ray_world = glm::normalize(ray_world);

	float t;
	float plane_distance = 0;

	t = -(dot(eye, up) + plane_distance) / (dot(ray_world, up));

	glm::vec3 ray_final = eye + ray_world * t;
	
	create_station(ray_final, sim, group_id);
}

glm::mat4 GL_Window::LookAt(glm::vec3 campos, glm::vec3 look, glm::vec3 up)
{
	// 새로운 축을 구한다
	// 원점으로 위치이동한다
	// 새로구한 축을 위치이동 해서 물체의 포지션과 곱한다 -> 물체의 각 좌표값을 내적한다 -> 그 좌표계에 따른 좌표를 구하는 것
	glm::mat4 matrix_LookAt = glm::mat4();

	glm::mat4 translation = glm::mat4();
	glm::mat4 rotation = glm::mat4();

	// translation matrix
	glm::vec4 v1 = glm::vec4(1, 0, 0, 0);
	glm::vec4 v2 = glm::vec4(0, 1, 0, 0);
	glm::vec4 v3 = glm::vec4(0, 0, 1, 0);
	glm::vec4 v4 = glm::vec4(-campos.x, -campos.y, -campos.z, 1);

	translation[0] = v1;
	translation[1] = v2;
	translation[2] = v3;
	translation[3] = v4;

	// new axis
	// 1. cam - target (camera는 - z축을 바라보고 있기 때문에 (-1) * (target - cam)
	glm::vec3 zaxis = glm::normalize(glm::vec3(campos - look));

	// 2. UpVector of camera 와 새로구한 z축의 외적 -> z축과 직교하는 또 하나의 축
	glm::vec3 xaxis = glm::normalize(glm::cross(up, zaxis));

	// 미리 구한 2개의 축을 외적 -> 마지막 축
	glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));

	// transpose
	v1 = glm::vec4(xaxis[0], yaxis[0], zaxis[0], 0);
	v2 = glm::vec4(xaxis[1], yaxis[1], zaxis[1], 0);
	v3 = glm::vec4(xaxis[2], yaxis[2], zaxis[2], 0);
	v4 = glm::vec4(0, 0, 0, 1);

	rotation[0] = v1;
	rotation[1] = v2;
	rotation[2] = v3;
	rotation[3] = v4;

	matrix_LookAt = rotation * translation;

	return matrix_LookAt;
}

glm::mat4 GL_Window::Perspective(float fovy, float aspect, float near_, float far_)
{
	glm::mat4 matrix_perspective;
	float harf_fovy_radian = glm::radians(fovy / 2);

	glm::vec4 v1 = glm::vec4(1 / (aspect * tan(harf_fovy_radian)), 0, 0, 0);
	glm::vec4 v2 = glm::vec4(0, 1 / tan(harf_fovy_radian), 0, 0);
	glm::vec4 v3 = glm::vec4(0, 0, -((far_ + near_) / (far_ - near_)), -((2 * far_ * near_) / (far_ - near_)));
	glm::vec4 v4 = glm::vec4(0, 0, -1, 0);

	matrix_perspective[0] = v1;
	matrix_perspective[1] = v2;
	matrix_perspective[2] = v3;
	matrix_perspective[3] = v4;

	matrix_perspective = glm::transpose(matrix_perspective);

	return matrix_perspective;
}

std::vector<glm::vec3> GL_Window::set_mesh(int num_particle, int r_x, int r_y, int r_z, float s)
{
	std::vector<glm::vec3> formation;
	std::vector<glm::vec3> dummy_mesh = mesh->meshEntries[0]->vertices;

	// setup
	glm::mat4 rotate_x = glm::rotate(glm::identity<glm::mat4>(), deg_to_rad(90.0f * r_x), glm::vec3(1, 0, 0));
	glm::mat4 rotate_y = glm::rotate(glm::identity<glm::mat4>(), deg_to_rad(90.0f * r_y), glm::vec3(0, 1, 0));
	glm::mat4 rotate_z = glm::rotate(glm::identity<glm::mat4>(), deg_to_rad(90.0f * r_z), glm::vec3(0, 0, 1));
	glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), VEC_ONE * s);

	// mesh를 회전, 크기조절 후 Pos를 바닥으로 Projection 
	for (auto pos = dummy_mesh.begin(); pos != dummy_mesh.end(); pos++)
	{
		glm::vec4 pos4 = glm::vec4(*pos, 0);

		pos4 = scale * pos4;  // 회전 위치 크기 조절

		pos4 = rotate_x * pos4;
		pos4 = rotate_y * pos4;
		pos4 = rotate_z * pos4;

		//pos4 = rotate_y * scale * pos4;  

		pos->x = pos4.x;
		pos->y = 0;
		pos->z = pos4.z;
	}

	// 중복 제거 : 정수는 1단위기 때문에 겹치지 않게 됨
	dummy_mesh = unique_vec3(dummy_mesh);
	int num_vertices = dummy_mesh.size();
	int div = num_vertices / num_particle;
	std::cout << "mesh vertices count : " << num_vertices << std::endl;
	std::cout << "div : " << div << std::endl;

	// formation pos
	for (int i = 0; i < num_vertices; i += div)
	{
		glm::vec4 pos = glm::vec4(dummy_mesh[i], 0);

		if (i / div < num_particle)
		{
			formation.push_back(pos);
		}
	}

	return formation;
}