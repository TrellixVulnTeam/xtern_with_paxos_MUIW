#!/usr/bin/perl

# Usage: ./draw-time-chart.pl <directory which stores a recorded schedule from one execution>
# E.g., ./draw-time-chart.pl ./out

$numThreads = 0;
%totalEvents;
%tids;
$curDir;
$threadMargin = "                                                  "; # 40 blanks.

sub parseSchedule {
	my $dirPath = $_[0];
	my $tid;
	my $op;
	my $instrId;
	my $turn;
	my $appTime = 0;
	my $syscallTime = 0;
	my $schedTime = 0;
	my $curTidTime = 0;
	my @fields1;
	my @fields2;
	
	chdir($dirPath);
	opendir (DIR, ".");
	while (my $file = readdir(DIR)) {
		next if ($file =~ m/^\./);
		#print "$file\n";

		@fields1 = split(/-/, $file);
		@fields2 = split(/\./, $fields1[2]);
		$tid = $fields2[0];
		next if ($tid eq 1);
		$curTidTime = 0;
		$tids{$tid} = 1;

		# Open the file and process it.
		open(LOG, $file);
		foreach $line (<LOG>) {
			next if ($line =~ m/^op/);
			@fields1 = split(/ /, $line);
			$op = $fields1[0];
			$instrId = $fields1[1];
			$turn = $fields1[2];
			$appTime = $fields1[3];
			$syscallTime = $fields1[4];
			$schedTime = $fields1[5];

			$appTime =~ s/:/\./g;
			$syscallTime =~ s/:/\./g;
			$schedTime =~ s/:/\./g;

			$curTidTime = $curTidTime + $appTime + $syscallTime + $schedTime;;
			
			#print "$turn:$tid:$appTime\n";
			$totalEvents{$turn} = $tid.":".$op.":".$curTidTime.":".$instrId;
		}
		close(LOG);
	}
}

sub updateGlobalTime {
	my @fields;
	my @fields2;
	my %doneTids;
	$doneTids{0} = 1;

	# For each thread id.
	for $key ( sort {$a<=>$b} keys %tids) {
		my $curTid = $key;
		next if ($curTid eq 0);
		next if ($tid eq 1);

		my $tidFirstEvent = 0;
		my $baseTime = 0;
		# For each event belongs to this thread id.
		for $turnKey ( sort {$a<=>$b} keys %totalEvents) {
			@fields = split(/:/, $totalEvents{$turnKey});
			my $tid = $fields[0];
			my $op = $fields[1];
			my $time = $fields[2];
			my $instrId = $fields[3];
			if ($curTid == $tid) {
				if ($tidFirstEvent == 0) {
					my $turn = $turnKey;
					while ($turn >= 0) {
						$turn--;
						@fields2 = split(/:/, $totalEvents{$turn});
						my $checkedTid = $fields2[0];
						if ($doneTids{$checkedTid} eq 1) { # Pick the latest done thread, use its time as base time of current thread.
							$baseTime = $fields2[2];
							$tidFirstEvent = 1;
							#print "Tid $curTid (turn $turnKey) setup a basetime $baseTime from tid $checkedTid (turn $turn)\n";
							last;
						}
					}
				}

				# Convert per-thread time to global time.
				$time = $baseTime + $time;
				$totalEvents{$turnKey} = $tid.":".$op.":".$time.":".$instrId;
			}
		}

		$doneTids{$curTid} = 1;
	}
}

# Print thread margin, CHART must be open when call this function.
sub printThreadMargin {
	my $tid = $_[0];
	my $i;
	if ($tid > 0) {
		for ($i = 0; $i < $tid-1; $i++) {
			print CHART $threadMargin;
		}
	}
}

sub drawTimeChart {
	my @fields;
	my $tid;
	my $op;
	my $finishTime;
	my $instrId;
	my $i;

	# Variables to draw time gaps.
	my $factor50 = 50;
	my $factor1K = 1000;
	my $gapSep50 = "========================";
	my $gapSep1K = "^^^^^^^^^^^^^^^^^^^^^^^^";
	my %curTime;
	my %prevTime;
	my %prevPrevTime;
	my $gap = 0;
	# Init.
	for $key ( sort {$a<=>$b} keys %tids) {
		$curTime{$key} = 0;
		$prevTime{$key} = 0;
		$prevPrevTime{$key} = 0;
	}

	# Print thread header.
	for $key ( sort {$a<=>$b} keys %tids) {
		print CHART "T".$key;
		# Print thread margin.
		print CHART $threadMargin;
	}
	print CHART "\n\n";
		
	#foreach $key (sort {$totalEvents{$a} <=> $totalEvents{$b} } keys %totalEvents) {
    	for $key ( sort {$a<=>$b} keys %totalEvents) {
    		@fields = split(/:/, $totalEvents{$key});
		#print CHART $key." --> ".$totalEvents{$key}."\n";
		$tid = $fields[0];
		$op = $fields[1];
		$finishTime = $fields[2];
		$instrId = $fields[3];

		# Update gap-variables, and print them if there is a big time gap.
		$prevPrevTime{$tid} = $prevTime{$tid};
		$prevTime{$tid} = $curTime{$tid};
		$curTime{$tid} = $finishTime;
		if (($curTime{$tid} - $prevTime{$tid}) > 0 && ($prevTime{$tid} - $prevPrevTime{$tid}) > 0) {
			$gap = ($curTime{$tid} - $prevTime{$tid})/($prevTime{$tid} - $prevPrevTime{$tid});
			if ($gap > $factor50 && $gap < $factor1K) {
				printThreadMargin($tid);
				print CHART $gapSep50.":".$gap."\n";
			} else {
				if ($gap > $factor1K) {
					printThreadMargin($tid);
					print CHART $gapSep1K.":".$gap."\n";
				}
			}
		}

		# Print op stat.
		printThreadMargin($tid);
		print CHART $op."(".$instrId.")"." ".$finishTime."\n";	
	}
}

$curDir = `pwd`;
parseSchedule(@ARGV[0]);
#print $curDir."\n";
chdir($curDir);
updateGlobalTime();

open(CHART, ">"."../time-chart.txt");
drawTimeChart();
close(CHART);

