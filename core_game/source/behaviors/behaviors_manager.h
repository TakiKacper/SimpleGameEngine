#pragma once
#include <string>
#include <memory>
#include "behavior_functions.h"
#include "behaviors_database.h"

namespace assets
{
	struct behavior;
	struct scene;
}

namespace entities
{
	class scene;
	class entity;

	namespace components
	{
		class behavior;
	}
}

namespace behaviors
{
	struct frame;
}

struct lua_State;

namespace behaviors
{
	/*
		behaviors_manager
		manages lua environement and execution of behaviors
	*/
	class behaviors_manager
	{
	friend assets::behavior;
	friend assets::scene;
	friend entities::scene;
	friend entities::components::behavior;
	friend database;
	public:
		behaviors_manager();
		~behaviors_manager();
		/*
			clear
		*/
		void clear();
		/*
			call_update_functions
			calls on_update on every registered behavior component
		*/
		void call_update_functions();
		/*
			register_behavior_component
			registered component will be updated every frame
		*/
		void register_behavior_component(entities::components::behavior* comp);
		/*
			unregister_behavior_component
			unregister the component so it is no longer updated
			also calls on_destroy on 
		*/
		void unregister_behavior_component(entities::components::behavior* comp);
		/*
			get_current_frame
			returns current frame
		*/
		const behaviors::frame* get_current_frame();
		/*
			require_module
			loads lua module (if not already loaded) and puts it on the stack
		*/
		void require_module(const std::string& path);
	private:
		struct implementation;
		implementation* impl;
		/*
			create_database
			creates new database in lua environement
			called by behaviors::database constructor
		*/
		int create_database();
		/*
			destroy_database
			removes all database data from lua environement
			called by behaviors::database destrutor
		*/
		void destroy_database(int id);
		/*
			create_functions_table
			loads behavior into lua virtual machine so it can be invoked
			-l-
			[file_path] path to lua file
			[result]	name of the table containging allocated functions
						required to call anything
		*/
		std::string create_functions_table(const std::string& file_path);
		/*
			destroy_functions_table
			frees table and functions created by create_functions_table
			-l-
			[table_name] name of the table, returned by create_functions_table
		*/
		void destroy_functions_table(const std::string& table_name);
		/*
			prepare_behavior_function_call
			informs lua environement about incoming behavior function call
			returns true if function can be called
			returns false if function is not implemented and futher actions should be abort
			-l-
			[func]	function to be invoken
			[bhv]	behavior from which function should be called
		*/
		bool prepare_behavior_function_call(behaviors::functions func, assets::behavior* bhv);
		/*
			prepare_custom_behavior_function_call
			informs lua environement about incoming behavior function call
			returns true if function can be called
			returns false if function is not implemented and futher actions should be abort
			-l-
			[func_name]			name of function to be invoken
			[bhv]				behavior from which function should be called
			[args_registry_id]	registry index of args table
		*/
		bool prepare_custom_behavior_function_call(const std::string& name, assets::behavior* bhv, const int& args_registry_id);
		/*
			prepare_scene_function_call
			informs lua environement about incoming scene function call
			returns true if function can be called
			returns false if function is not implemented and futher actions should be abort
			-l-
			[func_name]	name of function to be invoken
			[sc]		scene asset from whose function should be called
		*/
		bool prepare_scene_function_call(behaviors::functions func, assets::scene* sc);
		/*
			call
			calls behaviors function
			-l-
			[args_amount] amount of function arguments
			[results_amount] max amount of functions results
			[return value] amount of functions results limited by results_amount
		*/
		int call(int args_amount, int results_amount);
		void pass_entity_arg(std::weak_ptr<entities::entity>* entity);
		void pass_float_arg(float arg);
		void pass_nil();
		/*
			pass_custom_function_args
			loads args table back from registry
		*/
		void pass_custom_function_args(const int& args_registry_id);
		/*
			create_frame
			creates frame that holds program state
			-l-
			required before call
		*/
		void create_frame(std::shared_ptr<database> database, entities::scene* scene_context);
		/*
			pop_frame
			pops frame from the top of the stack
			-l-
			required after call
		*/
		void pop_frame();
	};
}