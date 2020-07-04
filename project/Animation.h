#pragma once
#include "Object.h"

class AnimateModel
{
public:
	vector<Model> models;
	int total;
	int start;
	int num;
	bool action;
	glm::vec3 Position;
	float TriggerDist = 2.0f;

	//读取路径，位置，总模型数，起始模型
	AnimateModel(glm::vec3 p, int totalnumber, int startnum)
	{
		total = totalnumber;
		num = start = startnum;
		Position = p;
		action = false;
	}
	void Load(string path)
	{
		string dictionary = path;
		for (int i = 1; i <= total; i++) {
			path = dictionary;
			path += to_string(i);
			path += ".obj";
			Model tmpmodel(path);
			models.push_back(tmpmodel);
		}

	};

	void Draw(Shader ourShader)
	{
		models[num].Draw(ourShader);
		Sleep(40);
		num++;
		if (num >= total)
			num = 0;

	}

};

class Door :public AnimateModel
{
public:

	Door(glm::vec3 p, int totalnumber, int startnum) : AnimateModel( p, totalnumber, startnum)
	{
	}

	void Update(glm::vec3 cPosition)
	{
		float dist = glm::distance(cPosition, Position);
		//cout << dist << endl;
		if (dist < TriggerDist && ((num == start)||(num == total +1)))
		{
			action = !action;
		}
	}
	void Draw(Shader shader)
	{

		if (!action) {
			if (num > start)
			{
				num--;
				models[num - 1].Draw(shader);
				Sleep(40);
			}
			else
			{
				models[0].Draw(shader);
				num = start;
			}
		}
		if (action && num <= total) {
			models[num - 1].Draw(shader);
			Sleep(40);
			num++;
		}
		if (action && num == total + 1) {
			models[total - 1].Draw(shader);
		}
	}

};

class Coin :public AnimateModel
{
public:
	bool firstAction;
	float Rotate;
	Coin( glm::vec3 p, int totalnumber, int startnum) : AnimateModel(p, totalnumber, startnum)
	{
		firstAction = false;
		TriggerDist = 2.0f;
		Rotate = 0.0f;
	}
	void Assign(glm::vec3 p, float r= 0)
	{
		Position = p;
		Rotate = r;
	}

	void Update(glm::vec3 cPosition, glm::vec3 cFront)
	{
		float dist = glm::distance(cPosition, Position);
		//cout << dist << endl;

		if (dist < TriggerDist)
		{
			if (glm::dot(Position - cPosition, cFront) >= 0)
			{
				if (!firstAction && !action )
				{
					firstAction = true;
					action = true;
				}
				else
				{
					firstAction = false;
					action = true;
				}

			}

		}
	}

	void Draw(Shader shader)
	{
		if (!action) {
			models[0].Draw(shader);
		}

		if (action && num <= total) {
			models[num - 1].Draw(shader);
			Sleep(40);
			num++;
		}

	}
};
