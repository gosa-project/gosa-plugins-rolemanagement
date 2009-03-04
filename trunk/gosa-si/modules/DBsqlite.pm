package GOSA::DBsqlite;

use strict;
use warnings;
use Carp;
use DBI;
use Data::Dumper;
use GOSA::GosaSupportDaemon;
use Time::HiRes qw(usleep);
use Fcntl ':flock'; # import LOCK_* constants

my $col_names = {};

sub new {
	my $class = shift;
	my $db_name = shift;

	my $lock = $db_name.".si.lock";
	my $self = {dbh=>undef,db_name=>undef,db_lock=>undef,db_lock_handle=>undef};
	my $dbh = DBI->connect("dbi:SQLite:dbname=$db_name", "", "", {RaiseError => 1, AutoCommit => 1});
	my $sth = $dbh->prepare("pragma integrity_check");
     $sth->execute();
	my @ret = $sth->fetchall_arrayref();
	if(length(@ret)==1 && $ret[0][0][0] eq 'ok') {
		&main::daemon_log("DEBUG: Database image $db_name is ok", 7);
	} else {
		&main::daemon_log("ERROR: Database image $db_name is malformed, creating new database.", 1);
		$sth->finish();
		$dbh->disconnect();
		unlink($db_name);
	  $dbh = DBI->connect("dbi:SQLite:dbname=$db_name", "", "", {RaiseError => 1, AutoCommit => 1});
	}
	$self->{dbh} = $dbh;
	$self->{db_name} = $db_name;
	$self->{db_lock} = $lock;
	bless($self,$class);
	return($self);
}


sub lock {
	my $self = shift;
	open($self->{db_lock_handle}, ">>".($self->{db_lock})) unless ref $self->{db_lock_handle};
	flock($self->{db_lock_handle},LOCK_EX);
	seek($self->{db_lock_handle}, 0, 2);
}


sub unlock {
	my $self = shift;
	flock($self->{db_lock_handle},LOCK_UN);
}


sub create_table {
	my $self = shift;
	my $table_name = shift;
	my $col_names_ref = shift;
	my @col_names;
	my @col_names_creation;
	foreach my $col_name (@$col_names_ref) {
		# Save full column description for creation of database
		push(@col_names_creation, $col_name);
		my @t = split(" ", $col_name);
		my $column_name = $t[0];
		# Save column name internally for select_dbentry
		push(@col_names, $column_name);
	}
	
	$col_names->{ $table_name } = \@col_names;
	my $col_names_string = join(", ", @col_names_creation);
	my $sql_statement = "CREATE TABLE IF NOT EXISTS $table_name ( $col_names_string )"; 
	$self->lock();
	eval {
		my $res = $self->{dbh}->do($sql_statement);
	};
	if($@) {
		$self->{dbh}->do("ANALYZE");
		eval {
			my $res = $self->{dbh}->do($sql_statement);
		};
		if($@) {
			&main::daemon_log("ERROR: $sql_statement failed with $@", 1);
		}
	}
	$self->unlock();

	return 0;
}


sub add_dbentry {
	my $self = shift;
	my $arg = shift;
	my $res = 0;   # default value

	# if dbh not specified, return errorflag 1
	my $table = $arg->{table};
	if( not defined $table ) { 
		return 1 ; 
	}

	# if timestamp is not provided, add timestamp   
	if( not exists $arg->{timestamp} ) {
		$arg->{timestamp} = &get_time;
	}

	# check primkey and run insert or update
	my $primkeys = $arg->{'primkey'};
	my $prim_statement="";
	if( 0 != @$primkeys ) {   # more than one primkey exist in list
		my @prim_list;
		foreach my $primkey (@$primkeys) {
			if( not exists $arg->{$primkey} ) {
				return (3, "primkey '$primkey' has no value for add_dbentry");
			}
			push(@prim_list, "$primkey='".$arg->{$primkey}."'");
		}
		$prim_statement = "WHERE ".join(" AND ", @prim_list);

		# check wether primkey is unique in table, otherwise return errorflag
		my $sql_statement = "SELECT * FROM $table $prim_statement";
		$self->lock();
		eval {
			$res = @{ $self->{dbh}->selectall_arrayref($sql_statement) };
		};
		if($@) {
			$self->{dbh}->do("ANALYZE");
			eval {
				$res = @{ $self->{dbh}->selectall_arrayref($sql_statement) };
			};
			if($@) {
				&main::daemon_log("ERROR: $sql_statement failed with $@", 1);
			}
		}
		$self->unlock();

	}

	# primkey is unique or no primkey specified -> run insert
	if ($res == 0) {
		# fetch column names of table
		my $col_names = &get_table_columns($self, $table);

		my $create_id=0;
		foreach my $col_name (@{$col_names}) {
			if($col_name eq "id" && (! exists $arg->{$col_name})) {
				#&main::daemon_log("0 DEBUG: id field found without value! Creating autoincrement statement!", 7);
				$create_id=1;
			}
		}

		# assign values to column name variables
		my @col_list;
		my @val_list;
		foreach my $col_name (@{$col_names}) {
			# use function parameter for column values
			if (exists $arg->{$col_name}) {
				push(@col_list, "'".$col_name."'");
				push(@val_list, "'".$arg->{$col_name}."'");
			}
		}    

		my $sql_statement;
		if($create_id==1) {
			$sql_statement = "INSERT INTO $table (id, ".join(", ", @col_list).") VALUES (null, ".join(", ", @val_list).")";
		} else {
			$sql_statement = "INSERT INTO $table (".join(", ", @col_list).") VALUES (".join(", ", @val_list).")";
		}
		my $db_res;
		$self->lock();
		eval {
			$db_res = $self->{dbh}->do($sql_statement);
		};
		if($@) {
			$self->{dbh}->do("ANALYZE");
			eval {
				$db_res = $self->{dbh}->do($sql_statement);
			};
			if($@) {
				&main::daemon_log("ERROR: $sql_statement failed with $@", 1);
			}
		}
		$self->unlock();

		if( $db_res != 1 ) {
			return (4, $sql_statement);
		} 

		# entry already exists -> run update
	} else  {
		my @update_l;
		while( my ($pram, $val) = each %{$arg} ) {
			if( $pram eq 'table' ) { next; }
			if( $pram eq 'primkey' ) { next; }
			push(@update_l, "$pram='$val'");
		}
		my $update_str= join(", ", @update_l);
		$update_str= " SET $update_str";

		my $sql_statement= "UPDATE $table $update_str $prim_statement";
		my $db_res = &update_dbentry($self, $sql_statement );
	}

	return 0;
}


sub update_dbentry {
	my ($self, $sql)= @_;
	my $db_answer= &exec_statement($self, $sql); 
	return $db_answer;
}


sub del_dbentry {
	my ($self, $sql)= @_;;
	my $db_res= &exec_statement($self, $sql);
	return $db_res;
}


sub get_table_columns {
	my $self = shift;
	my $table = shift;
	my @column_names;

	if(exists $col_names->{$table}) {
		@column_names = @{$col_names->{$table}};
	} else {
		my @res;
		$self->lock();
		eval {
			@res = @{$self->{dbh}->selectall_arrayref("pragma table_info('$table')")};
		};
		if($@) {
			$self->{dbh}->do("ANALYZE");
			eval {
				@res = @{$self->{dbh}->selectall_arrayref("pragma table_info('$table')")};
			};
			if($@) {
				&main::daemon_log("ERROR: pragma table_info('$table') failed with $@", 1);
			}
		}
		$self->unlock();

		foreach my $column (@res) {
			push(@column_names, @$column[1]);
		}
	}
	return \@column_names;

}


sub select_dbentry {
	my ($self, $sql)= @_;
	my $error= 0;
	my $answer= {};
	my $db_answer= &exec_statement($self, $sql); 
	my @column_list;

	# fetch column list of db and create a hash with column_name->column_value of the select query
	$sql =~ /SELECT ([\S\s]*?) FROM ([\S]*?)( |$)/g;
	my $selected_cols = $1;
	my $table = $2;

	# all columns are used for creating answer
	if ($selected_cols eq '*') {
		@column_list = @{ &get_table_columns($self, $table) };    

		# specific columns are used for creating answer
	} else {
		# remove all blanks and split string to list of column names
		$selected_cols =~ s/ //g;          
		@column_list = split(/,/, $selected_cols);
	}

	# create answer
	my $hit_counter = 0;
	my $list_len = @column_list;
	foreach my $hit ( @{$db_answer} ){
		$hit_counter++;
		for ( my $i = 0; $i < $list_len; $i++) {
			$answer->{ $hit_counter }->{ $column_list[$i] } = @{ $hit }[$i];
		}
	}

	return $answer;  
}


sub show_table {
	my $self = shift;
	my $table_name = shift;

	my $sql_statement= "SELECT * FROM $table_name ORDER BY timestamp";
	my $res= &exec_statement($self, $sql_statement);
	my @answer;
	foreach my $hit (@{$res}) {
		push(@answer, "hit: ".join(', ', @{$hit}));
	}

	return join("\n", @answer);
}


sub exec_statement {
	my $self = shift;
	my $sql_statement = shift;
	my @db_answer;

	$self->lock();
	eval {
		@db_answer = @{$self->{dbh}->selectall_arrayref($sql_statement)};
	};
	if($@) {
		$self->{dbh}->do("ANALYZE");
		eval {
			@db_answer = @{$self->{dbh}->selectall_arrayref($sql_statement)};
		};
		if($@) {
			&main::daemon_log("ERROR: $sql_statement failed with $@", 1);
		}
	}
	$self->unlock();
	# TODO : maybe an error handling and an erro feedback to invoking function
	#my $error = @$self->{dbh}->err;
	#if ($error) {
	#	my $error_string = @$self->{dbh}->errstr;
	#}

	return \@db_answer;
}


sub exec_statementlist {
	my $self = shift;
	my $sql_list = shift;
	my @db_answer;

	foreach my $sql (@$sql_list) {
		if(defined($sql) && length($sql) > 0) {
			# Obtain a new lock for each statement to not block the db for a too long time
			$self->lock();
			eval {
				my @answer = @{$self->{dbh}->selectall_arrayref($sql)};
				push @db_answer, @answer;
			};
			if($@) {
				$self->{dbh}->do("ANALYZE");
				eval {
					my @answer = @{$self->{dbh}->selectall_arrayref($sql)};
					push @db_answer, @answer;
				};
				if($@) {
					&main::daemon_log("ERROR: $sql failed with $@", 1);
				}
			}
			$self->unlock();
		} else {
			next;
		}
	}

	return \@db_answer;
}


sub count_dbentries {
	my ($self, $table)= @_;
	my $error= 0;
	my $count= -1;

	my $sql_statement= "SELECT count() FROM $table";
	my $db_answer= &select_dbentry($self, $sql_statement); 
	if(defined($db_answer) && defined($db_answer->{1}) && defined($db_answer->{1}->{'count()'})) {
		$count = $db_answer->{1}->{'count()'};
	}

	return $count;
}


sub move_table {
	my ($self, $from, $to) = @_;

	my $sql_statement_drop = "DROP TABLE IF EXISTS $to";
	my $sql_statement_alter = "ALTER TABLE $from RENAME TO $to";

	$self->lock();
	eval {
		$self->{dbh}->do($sql_statement_drop);
	};
	if($@) {
		$self->{dbh}->do("ANALYZE");
		eval {
			$self->{dbh}->do($sql_statement_drop);
		};
		if($@) {
			&main::daemon_log("ERROR: $sql_statement_drop failed with $@", 1);
		}
	}

	eval {
		$self->{dbh}->do($sql_statement_alter);
	};
	if($@) {
		$self->{dbh}->do("ANALYZE");
		eval {
			$self->{dbh}->do($sql_statement_alter);
		};
		if($@) {
			&main::daemon_log("ERROR: $sql_statement_alter failed with $@", 1);
		}
	}
	$self->unlock();

	return;
} 


1;