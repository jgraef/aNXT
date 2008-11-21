--    nxt_server_usage_example.adb 
--    example program to use the nxt_server program with Ada
--    libnxt - A C library for using LEGO Mindstorms NXT
--    Copyright (C) 2008  Janosch Graef <janosch.graef@gmx.net>
--    Copyright (C) 2008  J. "MUFTI" Scheurich (IITS Universitaet Stuttgart)
--    Copyright (C) 2008  Dr. Peter.Hermann (IHR Universitaet Stuttgart)
--
--    This program is free software: you can redistribute it and/or modify
--    it under the terms of the GNU General Public License as published by
--    the Free Software Foundation, either version 3 of the License, or
--    (at your option) any later version.
--
--    This program is distributed in the hope that it will be useful,
--    but WITHOUT ANY WARRANTY; without even the implied warranty of
--    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--    GNU General Public License for more details.
--
--    You should have received a copy of the GNU General Public License
--    along with this program.  If not, see <http://www.gnu.org/licenses/>.

with Interfaces.C; use Interfaces.C;

with Ada.Text_IO; use Ada.Text_IO;
with Ada.text_io.Unbounded_IO;
with Ada.Strings.Unbounded;
with Ada.Directories;

procedure nxt_server_usage_example is

   function Sys(arg : Char_Array) return Integer;
   pragma Import(C, Sys, "system"); -- executive request

   function Getpid return Integer; -- returns process-id(pid)
   pragma Import(C, Getpid, "getpid");

   pid : constant string := Integer'Image(Getpid);
   key : constant string := pid(pid'first+1 .. pid'last);

   nxt_server : constant string := "../../../bin/nxt_server";
   ret_val : constant Integer := Sys(To_C(nxt_server & " " & key  & " &"));
   
   nxt_base : constant string := "/tmp/nxt_server_";
   commandpipe : constant String := nxt_base & "in_" & key;
   errorPipe   : constant String := nxt_base & "err_" & key;
   resultPipe  : constant String := nxt_base & "out_" & key;

   command, error, result: File_Type;
   line : Ada.Strings.Unbounded.unbounded_string;

begin
   if ret_val = 0 then 
      while not Ada.Directories.Exists(commandpipe) loop
         delay 0.1;
      end loop;
      
      Create(command, Out_File, commandpipe);
      Open(error, In_File, errorpipe);
      Open(result, In_File, resultpipe);

      Put_Line(command, "1 *.*");
      Put_Line(command, "lsmod");
      Flush(command);

      Ada.text_io.Unbounded_IO.Get_Line(error, line);
      if Ada.Strings.Unbounded.length(line) /= 0 then
         Ada.text_io.Unbounded_IO.Put_Line(line);
      end if;

      loop 
         Ada.text_io.Unbounded_IO.Get_Line(result, line);
         exit when Ada.Strings.Unbounded.length(line) = 0; 
         Ada.text_io.Unbounded_IO.Put_Line(line);
      end loop;

      Put_Line(command, "exit");
      Flush(command);

      Close(command);
      Close(error);
      Close(result);
   end if;
end nxt_server_usage_example;
