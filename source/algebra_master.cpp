#include "algebra_master.h"

RealNumber* AlgebraMaster::solveEquationSystem(int size, RealNumber** matrix, const RealNumber* vector)
{
	//copy arguments
	RealNumber** m = new RealNumber*[size];
	for (int i = 0; i < size; i++)
	{
		m[i] = new RealNumber[size];
		for (int j = 0; j < size; j++)
			m[i][j] = matrix[i][j];
	}

	RealNumber* v = new RealNumber[size];
	for (int i = 0; i < size; i++)
		v[i] = vector[i];

	for (int column_index = 0; column_index < size; column_index++)
	{
		//find biggest in column
		int biggest_in_column_index = column_index;
		for (int j = column_index + 1; j < size; j++)
			if (abs(m[column_index][j]) > abs(m[column_index][biggest_in_column_index]))
				biggest_in_column_index = j;

		//faulty system
		if (m[column_index][biggest_in_column_index] == 0)
		{
			for (int i = 0; i < size; i++)
				delete[] m[i];
			delete[] m;
			delete[] v;
			return NULL;
		}

		//swap biggest to diagonal
		for (int i = 0; i < size; i++)
			common::swap<RealNumber>(m[i][column_index], m[i][biggest_in_column_index]);
		common::swap<RealNumber>(v[column_index], v[biggest_in_column_index]);

		//eliminate rows below
		for (int eliminated_row_index = column_index + 1; eliminated_row_index < size; eliminated_row_index++)
		{
			RealNumber factor = m[column_index][eliminated_row_index] / m[column_index][column_index];
			for (int i = column_index; i < size; i++)
				m[i][eliminated_row_index] -= factor*m[i][column_index];
			v[eliminated_row_index] -= factor*v[column_index];
		}
	}

	//estabilish result
	RealNumber* result = new RealNumber[size];
	for (int row_index = size - 1; row_index >= 0; row_index--)
	{
		for (int i = row_index + 1; i < size; i++)
			v[row_index] -= m[i][row_index] * result[i];
		result[row_index] = v[row_index] / m[row_index][row_index];
	}

	for (int i = 0; i < size; i++)
		delete[] m[i];
	delete[] m;
	delete[] v;

	return result;
}