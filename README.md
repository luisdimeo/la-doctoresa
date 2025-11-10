Este proyecto implementa un sistema de gestión hospitalaria en C++ que permite registrar pacientes y doctores, agendar y atender citas médicas, y consultar historiales clínicos. Está diseñado como una aplicación de consola interactiva, ideal para entornos educativos o como base para sistemas más complejos.

Funcionalidades principales
Registro de pacientes: Captura datos personales como nombre, cédula, tipo de sangre, dirección, teléfono, email, edad y sexo.

Registro de doctores: Incluye nombre, cédula profesional, especialidad, horario de atención, teléfono, email, experiencia y costo de consulta.

Agendar citas: Permite asignar citas entre pacientes y doctores, especificando fecha, hora y motivo.

Atender citas: Marca una cita como atendida, registra diagnóstico y costo, y lo agrega al historial médico del paciente.

Mostrar historial médico: Consulta el historial completo de un paciente por su ID.

Listar y buscar: Opciones para listar todos los pacientes y doctores, buscar por ID o por coincidencias parciales.

Eliminar registros: Permite eliminar pacientes o doctores del sistema.

 Estructura del código
Paciente: estructura que almacena información personal y su historial médico.

Doctor: estructura con datos profesionales y de contacto.

Cita: estructura que vincula pacientes y doctores con fecha, hora y motivo.

Historia
lMedico: estructura que almacena consultas atendidas.

Hospital: estructura principal que contiene arreglos de pacientes, doctores y citas, junto con contadores de capacidad.

 Menú interactivo
El programa presenta un menú principal con 13 opciones, desde registrar hasta eliminar entidades. Cada opción está implementada como un case dentro de un switch, y el menú se repite hasta que el usuario selecciona salir (opción 0).
