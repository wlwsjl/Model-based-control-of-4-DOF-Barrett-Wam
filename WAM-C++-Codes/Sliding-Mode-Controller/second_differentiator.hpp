/*
 * second_differentiator.hpp
 *
 *  Created on: 28-Feb-2015
 *      Author: nilxwam
 */

#ifndef SECOND_DIFFERENTIATOR_HPP_
#define SECOND_DIFFERENTIATOR_HPP_

#include <barrett/math/traits.h>
#include <list>
#include <barrett/units.h>
#include <barrett/detail/ca_macro.h>
#include <barrett/systems/abstract/system.h>

#include <eigen3/Eigen/Core>
#include <libconfig.h++>

#include <barrett/detail/ca_macro.h>
#include <barrett/math/traits.h>
#include <barrett/systems/abstract/execution_manager.h>
#include <barrett/systems/abstract/controller.h>
#include <samlibs.h>

using namespace barrett;
using namespace systems;

template<size_t DOF>
class second_differentiator: public System {

	BARRETT_UNITS_TEMPLATE_TYPEDEFS(DOF);

public:
	Input<double> time;
	Input<jp_type> inputSignal; // the input signal is either a jp_type , jv_type

public:
	Output<ja_type> outputSignal; // the output signal is either a jv_type , jv_type

protected:
	typename Output<ja_type>::Value* outputSignalOutputValue;

public:
	second_differentiator(int MODE) :
			time(this), inputSignal(this), outputSignal(this,
					&outputSignalOutputValue), mode(MODE) {
		container = Eigen::MatrixXd::Zero(mode, (int) DOF);
		time_vector = Eigen::Vector2d::Zero();
	}

	virtual ~second_differentiator() {
		this->mandatoryCleanUp();
	}

protected:
	int mode; // the mode of differentiator
	Eigen::MatrixXd container;
	int i, j;
	Eigen::RowVectorXd diff_output;
	Eigen::Vector2d time_vector;
	double del_time;
	ja_type tmp_out_value;
	double tmp_time;

	virtual void operate() {

		time_vector[0] = time_vector[1];
		time_vector[1] = this->time.getValue();
		del_time = time_vector[0] - time_vector[1];

		for (j = 0; j < mode - 1; j++) {
			for (i = 0; i < (int) DOF; i++) {
				container(j, i) = container(j + 1, i);
			}
		}
		for (i = 0; i < (int) DOF; i++) // updating the last entry
				{
			container(mode - 1, i) = this->inputSignal.getValue()[i];
		}

		if (mode == 5) // for mode 2
				{
			diff_output = (-2.0 * container.block(2, 0, 1, (int) DOF)
					+ container.block(0, 0, 1, (int) DOF)
					+ container.block(4, 0, 1, (int) DOF))
					/ (4.0 * del_time * del_time);

		}
		if (mode == 7) // for mode 3
				{
			diff_output = (-4.0 * container.block(3, 0, 1, (int) DOF)
					- (container.block(2, 0, 1, (int) DOF)
							+ container.block(4, 0, 1, (int) DOF))
					+ 2.0
							* (container.block(1, 0, 1, (int) DOF)
									+ container.block(5, 0, 1, (int) DOF))
					+ (container.block(0, 0, 1, (int) DOF)
							+ container.block(6, 0, 1, (int) DOF)))
					/ (16.0 * del_time * del_time);
		}

		if (mode == 9) // for mode 3
				{
			diff_output = (-10.0 * container.block(4, 0, 1, (int) DOF)
					- 4.0
							* (container.block(3, 0, 1, (int) DOF)
									+ container.block(5, 0, 1, (int) DOF))
					+ 4.0
							* (container.block(2, 0, 1, (int) DOF)
									+ container.block(6, 0, 1, (int) DOF))
					+ 4.0
							* (container.block(1, 0, 1, (int) DOF)
									+ container.block(7, 0, 1, (int) DOF))
					+ 1.0
							* (container.block(0, 0, 1, (int) DOF)
									+ container.block(8, 0, 1, (int) DOF)))
					/ (64.0 * del_time * del_time);
		}

		for (i = 1; i < (int) DOF; i++) {
			tmp_out_value[i] = diff_output[i];
		}

		this->outputSignalOutputValue->setData(&tmp_out_value);

	}
private:
	DISALLOW_COPY_AND_ASSIGN(second_differentiator);
}
;

#endif /* SECOND_DIFFERENTIATOR_HPP_ */
